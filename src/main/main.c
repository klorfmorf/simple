#include <ultra64.h>

#include "common.h"
#include "main/main.h"
#include "main/file.h"
#include "main/scheduler.h"
#include "main/memory.h"
#include "main/dram_stack.h"
#include "main/output_buffer.h"
#include "main/yield_data.h"
#include "main/text_labels.h"
#include "buffers/frame_buffer.h"
#include "buffers/depth_buffer.h"
#include "buffers/ascii_font_buffer.h"

OSMesg game_message = NULL;
u32 retraces_for_game_step = 0;
u32 is_processing_graphics_task = FALSE;
u32 retraces_since_last_graphics_task = 0;
u32 is_processing_game = FALSE;
u32 pending_graphics_tasks = 0;
u32 is_in_game_thread = FALSE;
u16 is_processing_reset = FALSE;
u16 current_frame_in_reset_process = 0;
u16 current_frame_in_reset_animation = 0;

u64 boot_thread_stack[0x2000 / sizeof(u64)];

// Probably an unused rmon stack.
u64 rmon_thread_stack[0x2000 / sizeof(u64)];
// OSThread rmon_thread;

u64 idle_thread_stack[0x2000 / sizeof(u64)];
OSThread idle_thread;

u64 main_thread_stack[0x2000 / sizeof(u64)];
OSThread main_thread;

u64 game_thread_stack[0x2000 / sizeof(u64)];
OSThread game_thread;

OSMesgQueue pi_message_queue;
OSMesg pi_message_buffer[PI_MESSAGE_COUNT];

OSMesgQueue dma_message_queue;
OSMesg dma_message_buffer[DMA_MESSAGE_COUNT];

OSMesgQueue main_message_queue;
OSMesg main_message_buffer[MAIN_MESSAGE_COUNT];

OSMesgQueue game_message_queue;
OSMesg game_message_buffer[GAME_MESSAGE_COUNT];

OSScMsg messages[TASK_COUNT];
OSScTask tasks[TASK_COUNT];

OSSched scheduler;
u64 scheduler_stack[OS_SC_STACKSIZE / sizeof(u64)];
OSMesgQueue* scheduler_command_queue_ptr;

System system;
System* system_ptr;

Gfx* display_list_head_ptr;

void idle_thread_function(void* argument);
void main_thread_function(void* argument);
void schedule_graphics_task();
void game_thread_function(void* argument);
void do_reset_animation();
void initialize_segments();
void initialize_display_lists();
void end_display_lists();
void clear_frame_buffer();
void clear_depth_buffer();
void dma_cartridge_to_ram(u32 rom_address, void* ram_address, u32 length);
void initialize_tasks(OSScTask* tasks, OSScMsg* messages, u64* microcode_text, u64* microcode_data, s32 unknown);

void boot(void* argument) {
    osInitialize();

    osInitialize_isv();

    osCreateThread(&idle_thread, 1, idle_thread_function, 0, (void*) idle_thread_stack + (0x2000 / sizeof(u64)), 9);
    osStartThread(&idle_thread);
}

void idle_thread_function(void* argument) {
    osCreatePiManager(OS_PRIORITY_PIMGR, &pi_message_queue, pi_message_buffer, PI_MESSAGE_COUNT);

    osCreateThread(&main_thread, 3, main_thread_function, argument, (void*) main_thread_stack + (0x2000 / sizeof(u64)), 10);
    osStartThread(&main_thread);

    osCreateThread(&game_thread, 4, game_thread_function, argument, (void*) game_thread_stack + (0x2000 / sizeof(u64)), 9);
    osStartThread(&game_thread);

    osSetThreadPri(0, 0);

    for (;;) {
        // Do nothing.
    }
}

extern u8 _graphics_buffersSegmentBssStart[];
extern u8 _overlay_bufferSegmentBssStart[];

extern void external_initialize();

void main_thread_function(void* argument) {
    s16 type;
    OSScClient main_client;
    OSScMsg* scheduler_message;

    type = 0;

    memory_initialize_system();
    initialize_tasks(tasks, messages, gspF3DEX_fifoTextStart, gspF3DEX_fifoDataStart, 0x8000);
    
    osCreateMesgQueue(&dma_message_queue, dma_message_buffer, DMA_MESSAGE_COUNT);
    osCreateMesgQueue(&main_message_queue, main_message_buffer, MAIN_MESSAGE_COUNT);
    osCreateMesgQueue(&game_message_queue, game_message_buffer, GAME_MESSAGE_COUNT);

    file_load(FILE_GRAPHICS_BUFFERS, _graphics_buffersSegmentBssStart);
    file_load(FILE_OVERLAY_BUFFER, _overlay_bufferSegmentBssStart);

    file_load(FILE_ASCII_FONT, (u8*) &ascii_font_buffer);

    osCreateScheduler(&scheduler, (void*) scheduler_stack + (0x2000 / sizeof(u64)), 13, OS_VI_NTSC_LAN1, 1);

    system_ptr->clear_depth_buffer = TRUE;
    system_ptr->retraces_per_game_step = 2;
    system_ptr->current_graphics_pool = 0;
    system_ptr->frames_elapsed = 0;
    system_ptr->current_frame_buffer = 1;
    system_ptr->disable_graphics_tasks = FALSE;

    osScAddClient(&scheduler, &main_client, &main_message_queue);
    scheduler_command_queue_ptr = osScGetCmdQ(&scheduler);

    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_GAMMA_DITHER_OFF | OS_VI_DIVOT_ON | OS_VI_DITHER_FILTER_ON);

    external_initialize();

    while (TRUE) {
        osRecvMesg(&main_message_queue, (OSMesg*) &scheduler_message, OS_MESG_BLOCK);

        type = scheduler_message->type;
        switch (type) {
            case OS_SC_RETRACE_MSG:
                system.retrace_count++;
                retraces_since_last_graphics_task++;

                if (is_processing_reset) {
                    do_reset_animation();
                } else {
                    retraces_for_game_step++;

                    if (system_ptr->retraces_per_game_step <= retraces_for_game_step && !is_in_game_thread) {
                        osSendMesg(&game_message_queue, &game_message, OS_MESG_NOBLOCK);
                        retraces_for_game_step = 0;
                    }
                }
                break;

            case OS_SC_DONE_MSG:
                is_processing_graphics_task = FALSE;

                if (!is_processing_reset && !is_processing_game) {
                    schedule_graphics_task();
                }
                break;
            
            case OS_SC_PRE_NMI_MSG:
                //! Turn of all sound...
                
                pending_graphics_tasks = 0;
                is_processing_reset = TRUE;
                break;
        }
    }
}

void schedule_graphics_task() {
    s16 index;
    OSMesgQueue* scheduler_interrupt_queue;

    if (!system_ptr->disable_graphics_tasks && pending_graphics_tasks) {
        index = system_ptr->current_graphics_pool;

        tasks[index].frame_buffer = frame_buffers[system_ptr->current_frame_buffer];

        is_processing_graphics_task = TRUE;
        pending_graphics_tasks = 0;
        retraces_since_last_graphics_task = 0;

        system_ptr->current_graphics_pool ^= 1;
        system_ptr->frames_elapsed++;
        system_ptr->current_frame_buffer++;

        if (system_ptr->current_frame_buffer > 2) {
            system_ptr->current_frame_buffer = 0;
        }

        scheduler_interrupt_queue = osScGetInterruptQ(&scheduler);
        osSendMesg(scheduler_interrupt_queue, (OSMesg) &tasks[index], OS_MESG_BLOCK);
    }
}

//TEMPORARY
const char *g_now_playing_strings[] = {
    "NOW PLAYING",
    "NOW PLAYINGx",
    "NOW PLAYINGxx",
    "NOW PLAYINGxxx",
};

void game_thread_function(void* argument) {
    while (TRUE) {
        osRecvMesg(&game_message_queue, &game_message, OS_MESG_BLOCK);

        is_in_game_thread = TRUE;
        is_processing_game = TRUE;

        display_list_head_ptr = system_ptr->graphics_pools[system_ptr->current_graphics_pool].display_lists;

        initialize_segments();
        initialize_display_lists();

        if (system_ptr->clear_depth_buffer) {
            clear_depth_buffer();
        }

        clear_frame_buffer();

        //! Read controller data...

        //! Do game loop stuff...
        text_labels_add_ascii_entry(0, g_now_playing_strings[(system_ptr->frames_elapsed / 15) % 4], 8, 8);

        if (!system_ptr->dont_process_draw_lists) {
            //! Process draw lists...

            text_labels_render();
        }


        end_display_lists();
        is_processing_game = FALSE;

        system_ptr->game_loops_ran++;
        is_in_game_thread = FALSE;
    }
}

void do_reset_animation() {
    s16 current_frame_buffer;
    u32 progress_lrx;
    u32 progress_ulx;
    u32 progress_lry;
    u32 progress_uly;

    if (current_frame_in_reset_process < 18) {
        current_frame_in_reset_process++;

        if (!is_processing_graphics_task || !is_in_game_thread) {
            if (current_frame_in_reset_animation == 0) {
                current_frame_buffer = system_ptr->current_frame_buffer;

                if (current_frame_buffer == 0) {
                    memory_copy((u8*) &frame_buffers[0], (u8*) &frame_buffers[1], sizeof(frame_buffers[0]));
                    memory_copy((u8*) &frame_buffers[0], (u8*) &frame_buffers[2], sizeof(frame_buffers[0]));
                } else if (current_frame_buffer == 1) {
                    memory_copy((u8*) &frame_buffers[1], (u8*) &frame_buffers[0], sizeof(frame_buffers[1]));
                    memory_copy((u8*) &frame_buffers[1], (u8*) &frame_buffers[2], sizeof(frame_buffers[1]));
                } else if (current_frame_buffer == 2) {
                    memory_copy((u8*) &frame_buffers[2], (u8*) &frame_buffers[0], sizeof(frame_buffers[2]));
                    memory_copy((u8*) &frame_buffers[2], (u8*) &frame_buffers[1], sizeof(frame_buffers[2]));
                }
            }

            current_frame_in_reset_animation++;

            display_list_head_ptr = system_ptr->graphics_pools[system_ptr->current_graphics_pool].display_lists;

            initialize_segments();
            initialize_display_lists();

            gDPSetScissor(display_list_head_ptr++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            gDPPipeSync(display_list_head_ptr++);
            gDPSetCycleType(display_list_head_ptr++, G_CYC_FILL);
            gDPSetColorImage(display_list_head_ptr++, G_IM_FMT_RGBA, system_ptr->frame_buffer_bits_per_pixel, SCREEN_WIDTH, OS_K0_TO_PHYSICAL(&frame_buffers[system_ptr->current_frame_buffer]));
            gDPSetFillColor(display_list_head_ptr++, 0x00010001);

            progress_lrx = current_frame_in_reset_animation * 10;
            if (progress_lrx >= SCREEN_WIDTH / 2) {
                progress_lrx = SCREEN_WIDTH / 2;
            }
            gDPFillRectangle(display_list_head_ptr++, 0, 0, progress_lrx, SCREEN_HEIGHT - 1);

            progress_ulx = SCREEN_WIDTH - (current_frame_in_reset_animation * 10);
            if (progress_ulx >= SCREEN_WIDTH) {
                progress_ulx = SCREEN_WIDTH - 1;
            }
            if (progress_ulx <= SCREEN_WIDTH / 2) {
                progress_ulx = SCREEN_WIDTH / 2;
            }
            gDPFillRectangle(display_list_head_ptr++, progress_ulx, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

            progress_lry = current_frame_in_reset_animation * 10;
            if (progress_lry >= (SCREEN_HEIGHT / 2) - 2) {
                progress_lry = (SCREEN_HEIGHT / 2) - 2;
            }
            gDPFillRectangle(display_list_head_ptr++, 0, 0, SCREEN_WIDTH - 1, progress_lry);

            progress_uly = SCREEN_HEIGHT - (current_frame_in_reset_animation * 10);
            if (progress_uly >= SCREEN_HEIGHT) {
                progress_uly = SCREEN_HEIGHT - 1;
            }
            if (progress_uly <= (SCREEN_HEIGHT / 2) + 2) {
                progress_uly = (SCREEN_HEIGHT / 2) + 2;
            }
            gDPFillRectangle(display_list_head_ptr++, 0, progress_uly, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

            gDPPipeSync(display_list_head_ptr++);

            end_display_lists();
            schedule_graphics_task();
            is_processing_graphics_task = FALSE;
        }

        if (current_frame_in_reset_animation == 17) {
            osViSetYScale(1.0f);

            //! Stop audio...

            system_ptr->fill_color_red = 0;
            system_ptr->fill_color_green = 0;
            system_ptr->fill_color_blue = 0;
        }
    }
}

void initialize_segments() {
    gSPSegment(display_list_head_ptr++, 0, 0);
    gSPSegment(display_list_head_ptr++, 1, OS_K0_TO_PHYSICAL(&depth_buffer));
    gSPSegment(display_list_head_ptr++, 2, OS_K0_TO_PHYSICAL(&frame_buffers[system_ptr->current_frame_buffer]));
    gSPSegment(display_list_head_ptr++, 4, OS_K0_TO_PHYSICAL(&system_ptr->graphics_pools[system_ptr->current_graphics_pool]));
    gSPSegment(display_list_head_ptr++, 5, OS_K0_TO_PHYSICAL(&ascii_font_buffer));
    //! gSPSegment(display_list_head_ptr++, 6, 0x325800); ?
    //! gSPSegment(display_list_head_ptr++, 9, 0x325800); ?
}

extern Gfx some_dl[];

void initialize_display_lists() {
    gSPDisplayList(display_list_head_ptr++, &some_dl);
}

void end_display_lists() {
    s16 index;

    gDPFullSync(display_list_head_ptr++);
    gSPEndDisplayList(display_list_head_ptr++);

    osWritebackDCache(&system_ptr->graphics_pools[system_ptr->current_graphics_pool], sizeof(GraphicsPool));

    index = system_ptr->current_graphics_pool;
    tasks[index].list.t.data_ptr = (u64*) system_ptr->graphics_pools[index].display_lists;
    tasks[index].list.t.data_size = (s32) (display_list_head_ptr - system_ptr->graphics_pools[system_ptr->current_graphics_pool].display_lists) * sizeof(Gfx);
    tasks[index].list.t.ucode_boot_size = 0xD0;
    tasks[index].list.t.output_buff = (u64*) &output_buffer;
    tasks[index].list.t.output_buff_size = (u64*) &output_buffer + (0x2380 / sizeof(u64));
    tasks[index].flags = OS_SC_RCP_MASK | OS_SC_LAST_TASK | OS_SC_SWAPBUFFER;

    pending_graphics_tasks++;

    if (!is_processing_graphics_task) {
        schedule_graphics_task();
    }
}

void clear_frame_buffer() {
    gDPPipeSync(display_list_head_ptr++);
    gDPSetCycleType(display_list_head_ptr++, G_CYC_FILL);
    gDPSetColorImage(display_list_head_ptr++, G_IM_FMT_RGBA, system_ptr->frame_buffer_bits_per_pixel, SCREEN_WIDTH, OS_K0_TO_PHYSICAL(&frame_buffers[system_ptr->current_frame_buffer]));
    gDPSetFillColor(display_list_head_ptr++, GPACK_RGBA5551(system_ptr->fill_color_red, system_ptr->fill_color_green, system_ptr->fill_color_blue, system_ptr->fill_color_alpha) << 16 | GPACK_RGBA5551(system_ptr->fill_color_red, system_ptr->fill_color_green, system_ptr->fill_color_blue, system_ptr->fill_color_alpha));
    gDPFillRectangle(display_list_head_ptr++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    gDPPipeSync(display_list_head_ptr++);
    gDPSetCycleType(display_list_head_ptr++, G_CYC_1CYCLE);
}

extern Gfx clear_depth_buffer_dl[];

void clear_depth_buffer() {
    gSPDisplayList(display_list_head_ptr++, &clear_depth_buffer_dl);
}

void dma_cartridge_to_ram(u32 rom_address, void* ram_address, u32 length) {
    OSIoMesg dma_io_message;
    OSMesg dma_message;
    s16 count;

    dma_message = 0;

    osInvalDCache(ram_address, length);
    osInvalICache(ram_address, length);

    osPiStartDma(&dma_io_message, 0, OS_READ, rom_address, ram_address, length, &dma_message_queue);
    osRecvMesg(&dma_message_queue, &dma_message, OS_MESG_BLOCK);
    
    // For some reason the game sleeps here...
    for (count = 4096; count != 0; count--) {
        // Do nothing...
    }
}

void initialize_tasks(OSScTask* tasks, OSScMsg* messages, u64* microcode_text, u64* microcode_data, s32 unknown) {
    s32 index;

    for (index = 0; index < TASK_COUNT; index++) {
        tasks[index].frame_buffer = frame_buffers[index];
        tasks[index].next = NULL;
        tasks[index].state = 0;
        tasks[index].flags = 0;
        tasks[index].list.t.type = M_GFXTASK;
        tasks[index].list.t.flags = OS_SC_NEEDS_RSP;
        tasks[index].list.t.ucode_boot = (u64*) &rspbootTextStart;
        tasks[index].list.t.ucode_boot_size = 0;
        tasks[index].list.t.ucode = microcode_text;
        tasks[index].list.t.ucode_size = SP_UCODE_SIZE;
        tasks[index].list.t.ucode_data = microcode_data;
        tasks[index].list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
        tasks[index].list.t.dram_stack = (u64*) &dram_stack;
        tasks[index].list.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
        tasks[index].list.t.output_buff = (u64*) &output_buffer;
        tasks[index].list.t.output_buff_size = 0;
        tasks[index].list.t.data_ptr = NULL;
        tasks[index].list.t.data_size = 0;
        tasks[index].list.t.yield_data_ptr = (u64*) &yield_data;
        tasks[index].list.t.yield_data_size = OS_YIELD_DATA_SIZE;
        tasks[index].msgQ = &main_message_queue;
        tasks[index].msg = (OSMesg) &messages[index];
    }

    if (unknown == 0) {
        messages[0].type = 0;
        messages[1].type = 0;
    } else {
        messages[0].type = OS_SC_DONE_MSG;
        messages[1].type = OS_SC_DONE_MSG;
    }
}

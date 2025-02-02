#ifndef MAIN_H
#define MAIN_H

#include <ultra64.h>

#include "common.h"
#include "main/scheduler.h"
#include "main/text_labels.h"

typedef struct {
    u16 status;
    u16 button_held_down;
    u16 button_pressed;
    s16 stick_raw_x;
    s16 stick_raw_y;
    u8 unknown[2];
    f32 stick_x;
    f32 stick_y;
    f32 stick_magnitude;
} Controller;

typedef struct {
    Gfx display_lists[10640];
    Mtx matrices[540];
    Vp viewports[4];
} GraphicsPool;

typedef struct {
    GraphicsPool graphics_pools[2];
    TextLabel text_labels[16];
    u16 retrace_count;
    u8 retraces_per_game_step;
    s16 disable_graphics_tasks;
    s16 current_graphics_pool;
    s16 current_frame_buffer;
    s16 frames_elapsed;
    u16 fill_color_red;
    u16 fill_color_green;
    u16 fill_color_blue;
    u16 fill_color_alpha;
    u16 frame_buffer_bits_per_pixel;
    s16 clear_depth_buffer;
    s16 dont_process_draw_lists;
    s16 game_loops_ran;
    u8 main_mode;
    u8 sub_mode;

} System;

extern OSMesgQueue dma_message_queue;
extern OSMesgQueue main_message_queue;
extern OSScMsg messages[TASK_COUNT];
extern OSSched scheduler;
extern OSMesgQueue* scheduler_command_queue_ptr;
extern System system;
extern System* system_ptr;
extern Gfx* display_list_head_ptr;

void dma_cartridge_to_ram(u32 rom_address, void* ram_address, u32 length);

#endif // MAIN_H

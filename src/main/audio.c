#ifdef TMP_AUDIO

#include <ultra64.h>

#include "common.h"
#include "main/audio.h"
#include "main/main.h"
#include "main/scheduler.h"

typedef struct 
{
    ALLink      node;
    int         startAddr;
    u32         lastFrame;
    char        *ptr;
} DMABuffer;

#define NBUFFERS 24
#define MAX_BUFFER_LENGTH 4096

u8* wave_table_offsets[AUDIO_BANK_COUNT] = {
    0x000000A0,
    0x000000B0,
    0x000000C0,
    0x000000D0,
    0x000000E0,
    0x000000F0,
    0x00000100,
    0x00000110,
};

u8* bank_file_start_offsets[AUDIO_BANK_COUNT] = {
    0x000000A0,
    0x000000B0,
    0x000000C0,
    0x000000D0,
    0x000000E0,
    0x000000F0,
    0x00000100,
    0x00000110,
};

u8* bank_file_end_offsets[AUDIO_BANK_COUNT] = {
    0x000000A0,
    0x000000B0,
    0x000000C0,
    0x000000D0,
    0x000000E0,
    0x000000F0,
    0x00000100,
    0x00000110,
};

#define ms * (((s32) ((f32) 44.1)) & ~0x7)

s32 custom_parameters[10] = {
    1, 300 ms,
    0, 179 ms, 12000, 0, 0x7FFF, 0, 0, 0
};

Acmd* audio_lists[2];
 
ALPlayer al_player;

u8 audio_heap[AUDIO_HEAP_SIZE];
ALBankFile* bank_files[AUDIO_BANK_COUNT];
ALHeap heap;

// DMABuffer   dmaBuffs[NBUFFERS];

s32 minimum_samples_per_frame;
s32 samples_per_frame;
s32 samples_for_buffer;

s32 audio_list_size;

OSMesgQueue audio_dma_message_queue;
OSMesg audio_dma_message_buffer[64];

ALGlobals audio_globals;

typedef struct {
    s16* sample_buffer;
    u16 sample_count;
    u16 unknown_1;
    OSScTask task;
} AudioTask;

AudioTask* audio_tasks[3];

// TEMPORARY
// OSThread audio_thread;

OSMesgQueue audio_retrace_message_queue;
OSMesg audio_retrace_message_buffer[64];

OSMesgQueue audio_rcp_done_message_queue;
OSMesg audio_rcp_done_message_buffer[64];

// TEMPORARY
// u64 audio_thread_stack[0x4000 / sizeof(u64)];

void audio_thread_function(void* argument);

void dmaproc() {

}

void create_audio_thread(ALSynConfig* synthesizer_config, OSPri priority, AudioConfig* audio_config) {
    s32 count;
    f32 calculated_samples_per_frame;
    s32 truncated_samples_per_frame;
    u32 buffer_index;
    u32 list_index;
    u32 task_index;

    synthesizer_config->dmaproc = &dmaproc;
    synthesizer_config->outputRate = osAiSetFrequency(audio_config->frequency);

    for (count = 16; count != 0; count--) {
        // Do nothing...
    }

    calculated_samples_per_frame = ((f32) audio_config->samples_per_frame_coefficient * (f32) synthesizer_config->outputRate) / 60.0f;
    truncated_samples_per_frame = (s32) calculated_samples_per_frame;

    samples_per_frame = truncated_samples_per_frame;

    if ((f32) truncated_samples_per_frame < calculated_samples_per_frame) {
        samples_per_frame += 1;
        truncated_samples_per_frame += 1;
    }

    if (truncated_samples_per_frame & 0xF) {
        truncated_samples_per_frame = (truncated_samples_per_frame & ~0xF) + 0x10;
        samples_per_frame = truncated_samples_per_frame;
    }

    minimum_samples_per_frame = truncated_samples_per_frame - 0x10;
    samples_for_buffer = truncated_samples_per_frame + 0x100;

    alInit(&audio_globals, synthesizer_config);

    // TEMPORARY
    /*

    dmaBuffs[0].node.prev = NULL;
    dmaBuffs[0].node.next = NULL;

    for (buffer_index = 0; buffer_index < (NBUFFERS - 1); buffer_index++) {
        alLink(&dmaBuffs[buffer_index + 1].node, &dmaBuffs[buffer_index].node);
        dmaBuffs[buffer_index].ptr = alHeapAlloc(synthesizer_config->heap, 1, MAX_BUFFER_LENGTH);
    }

    dmaBuffs[buffer_index].ptr = alHeapAlloc(synthesizer_config->heap, 1, MAX_BUFFER_LENGTH);

    */

    for (list_index = 0; list_index < 2; list_index++) {
        audio_lists[list_index] = alHeapAlloc(synthesizer_config->heap, 1, audio_config->audio_list_size * sizeof(Acmd));
    }
    
    audio_list_size = audio_config->audio_list_size;

    for (task_index = 0; task_index < 3; task_index++) {
        audio_tasks[task_index] = alHeapAlloc(synthesizer_config->heap, 1, 0x90);
        //audio_tasks[task_index]->unknown_2 = 2;
        //audio_tasks[task_index]->this = audio_tasks[task_index];
        audio_tasks[task_index]->sample_buffer = alHeapAlloc(synthesizer_config->heap, 1, samples_for_buffer * 4);
    }

    osCreateMesgQueue(&audio_rcp_done_message_queue, audio_rcp_done_message_buffer, 64);
    osCreateMesgQueue(&audio_retrace_message_queue, audio_retrace_message_buffer, 64);
    osCreateMesgQueue(&audio_dma_message_queue, audio_dma_message_buffer, 64);
    
    osCreateThread(&audio_thread, 3, audio_thread_function, 0, (void*) audio_thread_stack + (0x4000 / sizeof(u64)), priority);
    osStartThread(&audio_thread);
}

s32 send_audio_task(AudioTask* task_1, AudioTask* task_2);

s32 current_audio_task;

void audio_thread_function(void* argument) {
    OSScClient audio_client;
    OSScMsg* scheduler_message;
    s16 type;
    u8 stop;
    s32 audio_list_length;
    AudioTask* generated_audio_task;

    stop = FALSE;

    osScAddClient(&scheduler, &audio_client, &audio_retrace_message_queue);

    while (!stop) {
        osRecvMesg(&audio_retrace_message_queue, (OSMesg*) &scheduler_message, OS_MESG_BLOCK);
        type = scheduler_message->type;

        switch (type) {
            case OS_SC_RETRACE_MSG:
                audio_list_length = send_audio_task(audio_tasks[current_audio_task % 3], generated_audio_task);

                break;

            case OS_SC_PRE_NMI_MSG:
                break;

            case 0x20:
                stop = TRUE;
                break;
        }
    }

    alClose(&audio_globals);
}

s32 send_audio_task(AudioTask* task_1, AudioTask* task_2) {

}

void CleanDMABuffs(void);

#if 0
s32 dmaCallBack(s32 addr, s32 len, void *state)
{
    void        *freeBuffer;
    int         delta;
    DMABuffer   *dmaPtr,*lastDmaPtr;
    s32         addrEnd,buffEnd;


    lastDmaPtr = 0;
    dmaPtr = dmaState.firstUsed;
    addrEnd = addr+len;
 
    while(dmaPtr)  /* see if buffer is already set up */
    {

        buffEnd = dmaPtr->startAddr + MAX_BUFFER_LENGTH;
        if(dmaPtr->startAddr > addr) /* since buffers are ordered */
            break;                   /* abort if past possible */

        else if(addrEnd <= buffEnd) /* yes, found one */
        {
            dmaPtr->lastFrame = gFrameCt; /* mark it used */
            freeBuffer = dmaPtr->ptr + addr - dmaPtr->startAddr;
            return (int) osVirtualToPhysical(freeBuffer);
        }
        lastDmaPtr = dmaPtr;
        dmaPtr = (DMABuffer*)dmaPtr->node.next;
    }

    /* get here, and you didn't find a buffer, so dma a new one */

    /* get a buffer from the free list */
    dmaPtr = dmaState.firstFree;

#ifdef DEBUG
#ifndef __MWERKS__
    assert(dmaPtr);  /* be sure you have a buffer */
#endif
#endif

    dmaState.firstFree = (DMABuffer*)dmaPtr->node.next;
    alUnlink((ALLink*)dmaPtr);

    /* add it to the used list */
    if(lastDmaPtr) /* normal procedure */
    {
        alLink((ALLink*)dmaPtr,(ALLink*)lastDmaPtr);
    }
    else if(dmaState.firstUsed) /* jam at begining of list */
    {
        lastDmaPtr = dmaState.firstUsed;
        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = (ALLink*)lastDmaPtr;
        dmaPtr->node.prev = 0;
        lastDmaPtr->node.prev = (ALLink*)dmaPtr;
    }
    else /* no buffers in list, this is the first one */
    {
        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = 0;
        dmaPtr->node.prev = 0;
    }
    
    freeBuffer = dmaPtr->ptr;
    delta = addr & 0x1;
    addr -= delta;
    dmaPtr->startAddr = addr;
    dmaPtr->lastFrame = gFrameCt;  /* mark it */

    dmaIOMesgBuf[nextDMA].hdr.pri      = OS_MESG_PRI_NORMAL;
    dmaIOMesgBuf[nextDMA].hdr.retQueue = &dmaMessageQ;
    dmaIOMesgBuf[nextDMA].dramAddr     = freeBuffer;
    dmaIOMesgBuf[nextDMA].devAddr      = (u32)addr;
    dmaIOMesgBuf[nextDMA].size         = MAX_BUFFER_LENGTH;

    osEPiStartDma(handler, &dmaIOMesgBuf[nextDMA++], OS_READ);

    return (int) osVirtualToPhysical(freeBuffer) + delta;
}

ALDMAproc dmaNew(DMAState **state)
{
    int         i;

    if(!dmaState.initialized)  /* only do this once */
    {
        dmaState.firstFree = &dmaBuffs[0];
        for (i=0; i<NBUFFERS-1; i++)
        {
            alLink((ALLink*)&dmaBuffs[i+1],(ALLink*)&dmaBuffs[i]);
            dmaBuffs[i].ptr = alHeapAlloc(&hp, 1, MAX_BUFFER_LENGTH);
        }
	dmaBuffs[i].ptr = alHeapAlloc(&hp, 1, MAX_BUFFER_LENGTH);

        dmaState.initialized = 1;
    }

    *state = &dmaState;  /* state is never used in this case */

    return dmaCallBack;
}

void CleanDMABuffs(void)
{
    DMABuffer  *dmaPtr,*nextPtr;

    dmaPtr = dmaState.firstUsed;
    while(dmaPtr)
    {
        nextPtr = (DMABuffer*)dmaPtr->node.next;

        /* Can change this value.  Should be at least one.  */
        /* Larger values mean more buffers needed, but fewer DMA's */

        if(dmaPtr->lastFrame + 2  < gFrameCt) /* remove from used list */
        {
            if(dmaState.firstUsed == dmaPtr)
                dmaState.firstUsed = (DMABuffer*)dmaPtr->node.next;
            alUnlink((ALLink*)dmaPtr);
            if(dmaState.firstFree)
                alLink((ALLink*)dmaPtr,(ALLink*)dmaState.firstFree);
            else
            {
                dmaState.firstFree = dmaPtr;
                dmaPtr->node.next = 0;
                dmaPtr->node.prev = 0;
            }
        }
        dmaPtr = nextPtr;
    }
}
#endif

void audio_add_player(ALPlayer* player);

void audio_initialize() {
    s32 bank_file_length;
    s32 bank_file_length_masked;
    ALBankFile* bank_file_ptr;
    s32 bank_index;
    s32 data_index;
    ALSynConfig synthesizer_config;
    AudioConfig audio_config;

    alHeapInit(&heap, (u8*) audio_heap, AUDIO_HEAP_SIZE);

    for (bank_index = 0; bank_index < AUDIO_BANK_COUNT; bank_index++) {
        bank_file_length = bank_file_end_offsets[bank_index] - bank_file_start_offsets[bank_index];
        bank_file_ptr = (ALBankFile*) alHeapAlloc(&heap, 1, bank_file_length);
        bank_files[bank_index] = bank_file_ptr;

        data_index = 0;

        // Why is it done like this? Why doesn't this just iterate over the whole thing byte for byte? Inoue-san please explain...
        if (bank_file_length != 0) {
            bank_file_length_masked &= 3;

            if (bank_file_length_masked != 0) {
                for (; data_index < bank_file_length_masked; data_index++) {
                    ((u8*) bank_file_ptr)[data_index] = 0;
                }
            }
            
            for (; data_index < bank_file_length; data_index += 4) {
                ((u8*) bank_file_ptr)[data_index + 0] = 0;
                ((u8*) bank_file_ptr)[data_index + 1] = 0;
                ((u8*) bank_file_ptr)[data_index + 2] = 0;
                ((u8*) bank_file_ptr)[data_index + 3] = 0;
            }
        }
    }

    for (bank_index = 0; bank_index < AUDIO_BANK_COUNT; bank_index++) {
        alBnkfNew(bank_files[bank_index], wave_table_offsets[bank_index]);
    }

    synthesizer_config.maxVVoices = 16;
    synthesizer_config.maxPVoices = 16;
    synthesizer_config.maxUpdates = 160;
    synthesizer_config.dmaproc = NULL;
    synthesizer_config.outputRate = 0;
    synthesizer_config.heap = &heap;
    synthesizer_config.fxType = AL_FX_CUSTOM;
    synthesizer_config.params = custom_parameters;

    audio_config.frequency = 44100;
    audio_config.samples_per_frame_coefficient = 1;
    audio_config.audio_list_size = 0x1000;

    create_audio_thread(&synthesizer_config, 12, &audio_config);
    audio_add_player(&al_player);

}

void player_handler();

void audio_add_player(ALPlayer* player) {
    player->next = NULL;
    player->handler = (ALVoiceHandler) player_handler;
    player->clientData = player;
    alSynAddPlayer(&alGlobals->drvr, player);
}

void player_handler() {

}

#endif

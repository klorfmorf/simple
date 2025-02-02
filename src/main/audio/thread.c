#include <ultra64.h>

#include "common.h"
#include "main/main.h"
#include "main/scheduler.h"

#include "audio.h"

#define NBUFFERS 24
#define MAX_BUFFER_LENGTH 4096

/* ----- .data ----- */
u32 gFrameCt = 0;
s32 nextDMA = 0;
s32 curBuf = 0;

/* ----- .bss ----- */
Acmd *g_audio_lists[AUDIO_LIST_COUNT];
AudioTask *g_audio_tasks[AUDIO_TASK_COUNT];

OSThread g_audio_thread;

OSMesgQueue g_audio_retrace_message_queue;
OSMesg g_audio_retrace_message_buffer[RETRACE_MESSAGE_BUFFER_COUNT];

OSMesgQueue g_audio_task_message_queue;
OSMesg g_audio_task_message_buffer[TASK_MESSAGE_BUFFER_COUNT];

ALGlobals g_al_globals;

u64 g_audio_thread_stack[AUDIO_THREAD_STACK_COUNT / sizeof(u64)];

DMAState g_dma_state;
DMABuffer g_dma_buffers[DMA_BUFFER_COUNT];

s32 g_minimum_samples_per_frame;
s32 g_samples_per_frame;
s32 g_sample_buffer_size;
s32 g_audio_list_size;

OSIoMesg g_audio_dma_io_message_buffer[DMA_IO_MESSAGE_BUFFER_COUNT];

OSMesgQueue g_audio_dma_message_queue;
OSMesg g_audio_dma_message_buffer[DMA_MESSAGE_BUFFER_COUNT];

void thread_create(ALSynConfig *synthesizer_config, OSPri priority, AudioConfig *audio_config)
{
    s32 count;
    f32 frame_size_float;
    s32 frame_size_integer;
    u32 index;

    synthesizer_config->dmaproc = &dmaNew;
    synthesizer_config->outputRate = osAiSetFrequency(audio_config->frequency);

    for (count = 16; count != 0; count--)
    {
        // Do nothing...?
    }

    frame_size_float = (f32) (audio_config->samples_per_frame_coefficient * synthesizer_config->outputRate) / (f32) 60;
    frame_size_integer = (s32) frame_size_float;

    g_samples_per_frame = frame_size_integer;

    if (frame_size_integer < frame_size_float)
    {
        g_samples_per_frame++;
    }

    if (g_samples_per_frame & 0xF)
    {
        g_samples_per_frame = (g_samples_per_frame & ~0xF) + 0x10;
    }

    g_minimum_samples_per_frame = g_samples_per_frame - 0x10;
    g_sample_buffer_size = g_samples_per_frame + JITTER_SAMPLES;

    alInit(&g_al_globals, synthesizer_config);

    g_dma_buffers[0].node.prev = NULL;
    g_dma_buffers[0].node.next = NULL;

    for (index = 0; index < (DMA_BUFFER_COUNT - 1); index++)
    {
        alLink(&g_dma_buffers[index + 1].node, &g_dma_buffers[index].node);
        g_dma_buffers[index].ptr = alHeapAlloc(synthesizer_config->heap, 1, MAX_BUFFER_LENGTH);
    }

    g_dma_buffers[index].ptr = alHeapAlloc(synthesizer_config->heap, 1, MAX_BUFFER_LENGTH);

    for (index = 0; index < AUDIO_LIST_COUNT; index++)
    {
        g_audio_lists[index] = alHeapAlloc(synthesizer_config->heap, 1, audio_config->audio_list_size * sizeof(Acmd));
    }

    g_audio_list_size = audio_config->audio_list_size;

    for (index = 0; index < AUDIO_TASK_COUNT; index++)
    {
        g_audio_tasks[index] = alHeapAlloc(synthesizer_config->heap, 1, sizeof(AudioTask));
        ((AudioMessage *) &g_audio_tasks[index]->message)->type = 2;
        ((AudioMessage *) &g_audio_tasks[index]->message)->completed_task = g_audio_tasks[index];
        g_audio_tasks[index]->sample_buffer = alHeapAlloc(synthesizer_config->heap, 1, g_sample_buffer_size * 4);
    }

    osCreateMesgQueue(&g_audio_task_message_queue, g_audio_task_message_buffer, TASK_MESSAGE_BUFFER_COUNT);
    osCreateMesgQueue(&g_audio_retrace_message_queue, g_audio_retrace_message_buffer, RETRACE_MESSAGE_BUFFER_COUNT);
    osCreateMesgQueue(&g_audio_dma_message_queue, g_audio_dma_message_buffer, DMA_MESSAGE_BUFFER_COUNT);

    osCreateThread(&g_audio_thread, 3, thread_main, 0, (void *) g_audio_thread_stack + (AUDIO_THREAD_STACK_COUNT / sizeof(u64)), priority);
    osStartThread(&g_audio_thread);
}

void thread_main(void *argument)
{
    s16 type;
    u8 stop_thread;
    s32 audio_list_was_generated;
    AudioTask *completed_task;
    OSScClient audio_client;
    OSScMsg *scheduler_message;

    stop_thread = FALSE;
    completed_task = NULL;

    osScAddClient(&scheduler, &audio_client, &g_audio_retrace_message_queue);

    while (!stop_thread)
    {
        osRecvMesg(&g_audio_retrace_message_queue, (OSMesg *) &scheduler_message, OS_MESG_BLOCK);

        type = scheduler_message->type;
        switch (type)
        {
        case OS_SC_RETRACE_MSG:
            audio_list_was_generated = thread_generate_audio_task(g_audio_tasks[gFrameCt % 3], completed_task);

            if (audio_list_was_generated)
            {
                osRecvMesg(&g_audio_task_message_queue, (OSMesg *) &scheduler_message, OS_MESG_BLOCK);
                // Do some useless shit here...
                completed_task = ((AudioMessage *) scheduler_message)->completed_task;
            }
            break;

        case OS_SC_PRE_NMI_MSG:
            break;

        case 0x10:
            stop_thread = TRUE;
            break;
        }
    }

    alClose(&g_al_globals);
}

s32 thread_generate_audio_task(AudioTask *new_task, AudioTask *completed_task)
{
    s16 *sample_buffer;
    u32 remaining_bytes;
    Acmd *next_audio_list;
    s32 audio_list_length;

    CleanDMABuffs();

    sample_buffer = (s16 *) osVirtualToPhysical(new_task->sample_buffer);

    if (completed_task != NULL)
    {
        osAiSetNextBuffer(completed_task->sample_buffer, completed_task->sample_count * 4);
    }

    remaining_bytes = osAiGetLength();

    new_task->sample_count = ((g_samples_per_frame - (remaining_bytes / 4)) + JITTER_SAMPLES) & ~0xF;

    if (new_task->sample_count < g_minimum_samples_per_frame)
    {
        new_task->sample_count = g_minimum_samples_per_frame;
    }

    next_audio_list = alAudioFrame(g_audio_lists[curBuf], &audio_list_length, sample_buffer, new_task->sample_count);

    if (audio_list_length != 0)
    {
        new_task->task.next = NULL;
        new_task->task.msgQ = &g_audio_task_message_queue;
        new_task->task.msg = &new_task->message;
        new_task->task.flags = OS_SC_NEEDS_RSP;
        new_task->task.list.t.data_ptr = (u64 *) g_audio_lists[curBuf];
        new_task->task.list.t.data_size = (next_audio_list - g_audio_lists[curBuf]) * sizeof(Acmd);
        new_task->task.list.t.type = M_AUDTASK;
        new_task->task.list.t.ucode_boot = (u64 *) rspbootTextStart;
        new_task->task.list.t.ucode_boot_size = 0xD0;
        new_task->task.list.t.flags = 0x0;
        new_task->task.list.t.ucode = (u64 *) aspMainTextStart;
        new_task->task.list.t.ucode_size = 0x1000;
        new_task->task.list.t.ucode_data = (u64 *) aspMainDataStart;
        new_task->task.list.t.ucode_data_size = (aspMainDataEnd - aspMainDataStart) * sizeof(u64);
        new_task->task.list.t.yield_data_ptr = (u64 *) NULL;
        new_task->task.list.t.yield_data_size = 0;

        osSendMesg(scheduler_command_queue_ptr, (OSMesg) &new_task->task, OS_MESG_BLOCK);
        curBuf ^= 1;
    }

    return audio_list_length != 0;
}

s32 dmaCallBack(s32 addr, s32 len, void *state)
{
    void *freeBuffer;
    int delta;
    DMABuffer *dmaPtr, *lastDmaPtr;
    s32 addrEnd, buffEnd;

    lastDmaPtr = 0;
    dmaPtr = g_dma_state.firstUsed;
    addrEnd = addr + len;

    while (dmaPtr) /* see if buffer is already set up */
    {

        buffEnd = dmaPtr->startAddr + MAX_BUFFER_LENGTH;
        if (dmaPtr->startAddr > addr) /* since buffers are ordered */
            break;                    /* abort if past possible */

        else if (addrEnd <= buffEnd) /* yes, found one */
        {
            dmaPtr->lastFrame = gFrameCt; /* mark it used */
            freeBuffer = dmaPtr->ptr + addr - dmaPtr->startAddr;
            return (int)osVirtualToPhysical(freeBuffer);
        }
        lastDmaPtr = dmaPtr;
        dmaPtr = (DMABuffer *)dmaPtr->node.next;
    }

    /* get here, and you didn't find a buffer, so dma a new one */

    /* get a buffer from the free list */
    dmaPtr = g_dma_state.firstFree;

    // assert(dmaPtr); /* be sure you have a buffer */

    g_dma_state.firstFree = (DMABuffer *)dmaPtr->node.next;
    alUnlink((ALLink *)dmaPtr);

    /* add it to the used list */
    if (lastDmaPtr) /* normal procedure */
    {
        alLink((ALLink *)dmaPtr, (ALLink *)lastDmaPtr);
    }
    else if (g_dma_state.firstUsed) /* jam at begining of list */
    {
        lastDmaPtr = g_dma_state.firstUsed;
        g_dma_state.firstUsed = dmaPtr;
        dmaPtr->node.next = (ALLink *)lastDmaPtr;
        dmaPtr->node.prev = 0;
        lastDmaPtr->node.prev = (ALLink *)dmaPtr;
    }
    else /* no buffers in list, this is the first one */
    {
        g_dma_state.firstUsed = dmaPtr;
        dmaPtr->node.next = 0;
        dmaPtr->node.prev = 0;
    }

    freeBuffer = dmaPtr->ptr;
    delta = addr & 0x1;
    addr -= delta;
    dmaPtr->startAddr = addr;
    dmaPtr->lastFrame = gFrameCt; /* mark it */
    osPiStartDma(&g_audio_dma_io_message_buffer[nextDMA++], OS_MESG_PRI_NORMAL, OS_READ, (u32) addr, freeBuffer, MAX_BUFFER_LENGTH, &g_audio_dma_message_queue);

    return (int) osVirtualToPhysical(freeBuffer) + delta;
}

ALDMAproc dmaNew(DMAState **state)
{
    if (!g_dma_state.initialized) /* only do this once */
    {
        g_dma_state.firstUsed = NULL;
        g_dma_state.firstFree = &g_dma_buffers[0];
        g_dma_state.initialized = 1;
    }

    *state = &g_dma_state; /* state is never used in this case */

    return dmaCallBack;
}

void CleanDMABuffs(void)
{
    DMABuffer *dmaPtr, *nextPtr;
    u32 index;
    u8 message[8];

    if (nextDMA != 0)
    {
        for (index = 0; index < nextDMA; index++)
        {
            osRecvMesg(&g_audio_dma_message_queue, (OSMesg *) message, OS_MESG_NOBLOCK);
        }
    }
    
    dmaPtr = g_dma_state.firstUsed;
    while (dmaPtr)
    {
        nextPtr = (DMABuffer *)dmaPtr->node.next;

        /* Can change this value.  Should be at least one.  */
        /* Larger values mean more buffers needed, but fewer DMA's */

        if (dmaPtr->lastFrame + 14 < gFrameCt) /* remove from used list */
        {
            if (g_dma_state.firstUsed == dmaPtr)
                g_dma_state.firstUsed = (DMABuffer *)dmaPtr->node.next;
            alUnlink((ALLink *)dmaPtr);
            if (g_dma_state.firstFree)
                alLink((ALLink *)dmaPtr, (ALLink *)g_dma_state.firstFree);
            else
            {
                g_dma_state.firstFree = dmaPtr;
                dmaPtr->node.next = 0;
                dmaPtr->node.prev = 0;
            }
        }
        dmaPtr = nextPtr;
    }

    nextDMA = 0;
    gFrameCt++;
}

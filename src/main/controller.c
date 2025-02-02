#include <ultra64.h>

OSMesgQueue controller_message_queue;
OSMesg controller_message_buffer;

OSContStatus controller_status[MAXCONTROLLERS];

void controller_initialize() {
    s32 index;
    u8 bit_pattern;
    OSMesgQueue serial_message_queue;
    OSMesg serial_message;

    osCreateMesgQueue(&serial_message_queue, &serial_message, 1);
    osSetEventMesg(OS_EVENT_SI, &serial_message_queue, (OSMesg) 1);

    osContInit(&serial_message_queue, &bit_pattern, &controller_status[0]);

    osCreateMesgQueue(&controller_message_queue, &controller_message_buffer, 1);
    osSetEventMesg(OS_EVENT_SI, &controller_message_queue, (OSMesg) 1);

    for (index = 0; index < MAXCONTROLLERS; index++) {
        if ((bit_pattern & (1 << index)) || !(controller_status[index].errno & CONT_NO_RESPONSE_ERROR)) {
        }
    }
}
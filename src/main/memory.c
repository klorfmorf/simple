#include <ultra64.h>

#include "main/memory.h"
#include "main/main.h"

void memory_initialize_system() {
    memory_clear((u8*) &system, sizeof(System));

    system_ptr = &system;
    system.fill_color_alpha = 1;
    system.frame_buffer_bits_per_pixel = G_IM_SIZ_16b;
}

void memory_clear(u8* source, s32 size) {
    s32 index;

    for (index = 0; index < size; index++) {
        source[index] = 0;
    }
}

void memory_copy(u8* source, u8* destination, s32 size) {
    s32 index;

    for (index = 0; index < size; index++) {
        source[index] = destination[index];
    }
}

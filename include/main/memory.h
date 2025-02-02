#ifndef MEMORY_H
#define MEMORY_H

#include <ultra64.h>

void memory_initialize_system();
void memory_clear(u8* source, s32 size);
void memory_copy(u8* source, u8* destination, s32 size);

#endif // MEMORY_H

#ifndef YIELD_DATA_H
#define YIELD_DATA_H

#include <ultra64.h>

// For some reason this is 0x1000 instead of 0xC00... u64 yield_data[OS_YIELD_DATA_SIZE / sizeof(u64)];
extern u64 yield_data[0x1000 / sizeof(u64)];

#endif // YIELD_DATA_H

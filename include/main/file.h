#ifndef FILE_H
#define FILE_H

#include <ultra64.h>

enum {
    FILE_NONE,
    FILE_ASCII_FONT,
    FILE_GRAPHICS_BUFFERS,
    FILE_MISCELLANEOUS_BUFFERS,
    FILE_OVERLAY_BUFFER,
    FILE_MAX,
};

typedef struct {
    void* segment_start;
    void* segment_end;
} FileSegment;

void* file_load(u32 file_id, u8* ram_address);

#endif // FILE_H

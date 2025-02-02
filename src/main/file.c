#include <ultra64.h>

#include "main/file.h"
#include "main/main.h"

extern u8 _ascii_fontSegmentStart[], _ascii_fontSegmentEnd[];
extern u8 _graphics_buffersSegmentBssStart[], _graphics_buffersSegmentBssEnd[];
extern u8 _miscellaneous_buffersSegmentBssStart[], _miscellaneous_buffersSegmentBssEnd[];
extern u8 _overlay_bufferSegmentBssStart[], _overlay_bufferSegmentBssEnd[];

extern u8 _ascii_fontSegmentRomStart[], _ascii_fontSegmentRomEnd[];
extern u8 _RomSize[];

FileSegment file_segments[] = {
    { _ascii_fontSegmentStart, _ascii_fontSegmentEnd },
    { _graphics_buffersSegmentBssStart, _graphics_buffersSegmentBssEnd },
    { _miscellaneous_buffersSegmentBssStart, _miscellaneous_buffersSegmentBssEnd },
    { _overlay_bufferSegmentBssStart, _overlay_bufferSegmentBssEnd },
};

char file_offsets_marker_string[] = "Nisitenma-Ichigo";

u32 file_offsets[] = {
    (u32) _ascii_fontSegmentRomStart,
    (u32) _ascii_fontSegmentRomEnd,
    (u32) _ascii_fontSegmentRomEnd,
    (u32) _ascii_fontSegmentRomEnd,

    // End of the table, always the offset of the end of the last file in ROM.
    (u32) _RomSize,
};

u8 is_loading_file;

void* file_load(u32 file_id, u8* buffer_start) {
    s32 current_file_index;
    s32 next_file_index;
    u32 file_rom_offset;
    u32 file_length;
    u32 file_length_aligned;
    u8* buffer_current;
    u8* buffer_end;
    u8 was_loading_file;

    was_loading_file = is_loading_file;

    if (file_id < FILE_MAX) {
        if (file_id != FILE_NONE) {
            current_file_index = file_id - 1;
            next_file_index = file_id;

            file_rom_offset = file_offsets[current_file_index] & 0x7FFFFFFF;
            file_length = (file_offsets[next_file_index] & 0x7FFFFFFF) - file_rom_offset;

            buffer_end = buffer_start + ((u32) file_segments[current_file_index].segment_start - (u32) file_segments[current_file_index].segment_end);

            is_loading_file = TRUE;

            buffer_current = buffer_start;

            if (file_length != 0) {
                // This statement is equivalent to `file_offsets[current_file_index] & 0x80000000`.
                if ((s32) file_offsets[current_file_index] < 0) {
                    //! memory_decompress(file_rom_offset, (void*) buffer_start, (file_offsets[next_file_index] & 0x7FFFFFFF) - file_rom_offset); ??? Repeats this statement for some reason.
                    //! memory_decompress(file_rom_offset, (void*) buffer_start, file_length);
                } else {
                    file_length_aligned = (file_length + 1) & 0xFFFFFFFE;

                    dma_cartridge_to_ram(file_rom_offset, (void*) buffer_start, file_length_aligned);

                    buffer_current = buffer_start + file_length_aligned;
                }
            }

            is_loading_file = was_loading_file;

            for (; buffer_current < buffer_end; buffer_current++) {
                *buffer_current = 0;
            }

            osWritebackDCache(buffer_start, buffer_end - buffer_start);
        }
    } else {
        buffer_end = NULL;
    }

    return buffer_end;
}
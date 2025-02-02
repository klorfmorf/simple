#include <ultra64.h>

#include "main/text_labels.h"
#include "main/main.h"
#include "buffers/ascii_font_buffer.h"

extern Gfx a_dl[];

u16 ascii_font_palette[] = {
    0x0000, 0x1001, 0x2001, 0x3001,
    0x4001, 0x5001, 0x6001, 0x39D1,
    0x8001, 0x9001, 0xA001, 0xB001,
    0xC001, 0xD001, 0xE001, 0xFFFF,
};

TextureOffsets ascii_texture_offsets[] = {
    { 0x0000, 0x0000 },
    { 0x0100, 0x0000 },
    { 0x0200, 0x0000 },
    { 0x0300, 0x0000 },
    { 0x0400, 0x0000 },
    { 0x0500, 0x0000 },
    { 0x0600, 0x0000 },
    { 0x0700, 0x0000 },
    { 0x0800, 0x0000 },
    { 0x0900, 0x0000 },
    { 0x0F00, 0x0300 },
    { 0x0F00, 0x0300 },
    { 0x0F00, 0x0300 },
    { 0x0F00, 0x0300 },
    { 0x0F00, 0x0300 },
    { 0x0F00, 0x0300 },
    { 0x0E00, 0x0300 },
    { 0x0A00, 0x0000 },
    { 0x0B00, 0x0000 },
    { 0x0C00, 0x0000 },
    { 0x0D00, 0x0000 },
    { 0x0E00, 0x0000 },
    { 0x0F00, 0x0000 },
    { 0x0000, 0x0100 },
    { 0x0100, 0x0100 },
    { 0x0200, 0x0100 },
    { 0x0300, 0x0100 },
    { 0x0400, 0x0100 },
    { 0x0500, 0x0100 },
    { 0x0600, 0x0100 },
    { 0x0700, 0x0100 },
    { 0x0800, 0x0100 },
    { 0x0900, 0x0100 },
    { 0x0A00, 0x0100 },
    { 0x0B00, 0x0100 },
    { 0x0C00, 0x0100 },
    { 0x0D00, 0x0100 },
    { 0x0E00, 0x0100 },
    { 0x0F00, 0x0100 },
    { 0x0000, 0x0200 },
    { 0x0100, 0x0200 },
    { 0x0200, 0x0200 },
    { 0x0300, 0x0200 },
    { 0x0F00, 0x0300 },
    { 0x0F00, 0x0300 },
    { 0x0F00, 0x0300 },
    { 0x0F00, 0x0300 },
    { 0x0F00, 0x0300 },
    { 0x0F00, 0x0300 },
    { 0x0400, 0x0200 },
    { 0x0500, 0x0200 },
    { 0x0600, 0x0200 },
    { 0x0700, 0x0200 },
    { 0x0800, 0x0200 },
    { 0x0900, 0x0200 },
    { 0x0A00, 0x0200 },
    { 0x0B00, 0x0200 },
    { 0x0C00, 0x0200 },
    { 0x0D00, 0x0200 },
    { 0x0E00, 0x0200 },
    { 0x0F00, 0x0200 },
    { 0x0000, 0x0300 },
    { 0x0100, 0x0300 },
    { 0x0200, 0x0300 },
    { 0x0300, 0x0300 },
    { 0x0400, 0x0300 },
    { 0x0500, 0x0300 },
    { 0x0600, 0x0300 },
    { 0x0700, 0x0300 },
    { 0x0800, 0x0300 },
    { 0x0900, 0x0300 },
    { 0x0A00, 0x0300 },
    { 0x0B00, 0x0300 },
    { 0x0C00, 0x0300 },
    { 0x0D00, 0x0300 },
};

TextureOffsets hexadecimal_texture_offsets[] = {
    { 0x0000, 0x0000 },
    { 0x0100, 0x0000 },
    { 0x0200, 0x0000 },
    { 0x0300, 0x0000 },
    { 0x0400, 0x0000 },
    { 0x0500, 0x0000 },
    { 0x0600, 0x0000 },
    { 0x0700, 0x0000 },
    { 0x0800, 0x0000 },
    { 0x0900, 0x0000 },
    { 0x0A00, 0x0000 },
    { 0x0B00, 0x0000 },
    { 0x0C00, 0x0000 },
    { 0x0D00, 0x0000 },
    { 0x0E00, 0x0000 },
    { 0x0F00, 0x0000 },
};

void text_labels_clear() {
    s32 index;

    for (index = 0; index < 16; index++) {
        system_ptr->text_labels[index].type = 0;
    }
}

void text_labels_clear_entry(s16 entry) {
    system_ptr->text_labels[entry].type = 0;
}

void text_labels_add_ascii_entry(s16 entry, const char* string, s16 x, s16 y) {
    system_ptr->text_labels[entry].type = TEXT_LABEL_STRING;
    system_ptr->text_labels[entry].x = x;
    system_ptr->text_labels[entry].y = y;
    system_ptr->text_labels[entry].string = string;
}

void text_labels_add_hexadecimal_entry(s16 entry, u32 number, s16 x, s16 y) {
    system_ptr->text_labels[entry].type = TEXT_LABEL_HEXADECIMAL;
    system_ptr->text_labels[entry].x = x;
    system_ptr->text_labels[entry].y = y;
    system_ptr->text_labels[entry].number = number;
}

void text_labels_render() {
    s16 index;
    s32 nibble;
    u8 type;
    const char* string;
    char character;
    s32 x_position_low;
    s32 y_position_low;
    s32 x_position_high;
    s32 y_position_high;
    s32 x_offset;
    s32 offsets_index;
    s16 s_offset;
    s16 t_offset;

    gSPDisplayList(display_list_head_ptr++, &a_dl);
    gDPLoadTLUT_pal16(display_list_head_ptr++, 0, &ascii_font_palette);
    gDPLoadTextureBlock_4b(display_list_head_ptr++, &ascii_font_buffer, G_IM_FMT_CI, 128, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    for (index = 0; index < 16; index++) {
        type = system_ptr->text_labels[index].type;

        if (type != TEXT_LABEL_NONE) {
            x_offset = 0;

            if (type == TEXT_LABEL_STRING) {
                string = system_ptr->text_labels[index].string;
                character = *string++;

                while (character != 0) {
                    if (character != 0x20) {
                        x_position_low = system_ptr->text_labels[index].x + x_offset;
                        y_position_low = system_ptr->text_labels[index].y;

                        x_position_high = x_position_low + 7;
                        y_position_high = y_position_low + 7;
                        
                        offsets_index = character - 0x30;
                        s_offset = ascii_texture_offsets[offsets_index].s;
                        t_offset = ascii_texture_offsets[offsets_index].t;

                        gSPTextureRectangle(display_list_head_ptr++, x_position_low << 2, y_position_low << 2, x_position_high << 2, y_position_high << 2, G_TX_RENDERTILE, s_offset, t_offset, 0x1000, 0x0400);
                    }

                    x_offset += 8;
                    character = *string++;
                }
            } else if (type == TEXT_LABEL_HEXADECIMAL) {
                for (nibble = 0; nibble < 8; nibble++) {
                    x_position_low = system_ptr->text_labels[index].x + x_offset;
                    y_position_low = system_ptr->text_labels[index].y;

                    x_position_high = x_position_low + 7;
                    y_position_high = y_position_low + 7;
                    
                    offsets_index = (system_ptr->text_labels[index].number >> (((7 - nibble) * 4) & 0x1F)) & 0xF;
                    s_offset = hexadecimal_texture_offsets[offsets_index].s;
                    t_offset = hexadecimal_texture_offsets[offsets_index].t;

                    gSPTextureRectangle(display_list_head_ptr++, x_position_low << 2, y_position_low << 2, x_position_high << 2, y_position_high << 2, G_TX_RENDERTILE, s_offset, t_offset, 0x1000, 0x0400);

                    x_offset += 8;
                }
            }
        }
    }
}

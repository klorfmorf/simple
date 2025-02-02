#ifndef TEXT_LABELS_H
#define TEXT_LABELS_H

#include <ultra64.h>

enum {
    TEXT_LABEL_NONE,
    TEXT_LABEL_STRING,
    TEXT_LABEL_HEXADECIMAL,
};

typedef struct {
    u8 type;
    u8 unused_1;
    s16 x;
    s16 y;
    s16 unused_2;
    const char* string; // Used when displaying a string.
    u32 number; // Used when displaying a number.
} TextLabel;

typedef struct {
    s16 s;
    s16 t;
} TextureOffsets;

void text_labels_clear();
void text_labels_clear_entry(s16 entry);
void text_labels_add_ascii_entry(s16 entry, const char* string, s16 x, s16 y);
void text_labels_add_hexadecimal_entry(s16 entry, u32 number, s16 x, s16 y);
void text_labels_render();

#endif // TEXT_LABELS_H

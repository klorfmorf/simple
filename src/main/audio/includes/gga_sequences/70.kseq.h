u8 seq_data[] =
{
    0x00, 0x10, 0x00, 0x54, 0x00, 0xC6, 0x01, 0x39, 0x01, 0x8D, 0x02, 0x44, 0x02, 0xED, 0x97, 0x08, 
    0xDA, 0xB3, 0x78, 0xD0, 0x72, 0xD5, 0xDB, 0xDD, 0xC0, 0xEF, 0x00, 0xD1, 0xC0, 0x5C, 0x48, 0x30, 
    0x7F, 0x60, 0x49, 0x28, 0xD2, 0x48, 0x08, 0xC8, 0x48, 0x18, 0xE0, 0xB0, 0xE0, 0x49, 0x30, 0xD2, 
    0xFA, 0x2C, 0xD1, 0xC0, 0x22, 0xDF, 0xFE, 0xB0, 0xE0, 0xDF, 0xFA, 0x49, 0x28, 0xD2, 0xDF, 0xF8, 
    0x48, 0x08, 0xC8, 0xDF, 0xF6, 0x48, 0x18, 0xE0, 0xDF, 0xF4, 0xB0, 0xE0, 0xDF, 0xF2, 0x49, 0x36, 
    0xD2, 0xF1, 0x00, 0xFF, 0xD0, 0x72, 0xD4, 0xD7, 0x12, 0x99, 0xD9, 0x1C, 0xD8, 0x10, 0xEF, 0x04, 
    0xD1, 0xC0, 0x5C, 0x18, 0x30, 0x7F, 0x60, 0xE4, 0x18, 0x18, 0x30, 0xF2, 0x78, 0xDF, 0xFF, 0x18, 
    0x18, 0xE0, 0xDF, 0xFE, 0xD1, 0xC0, 0x22, 0x18, 0x30, 0xE0, 0xE4, 0x18, 0x18, 0x30, 0xDF, 0xFA, 
    0xF2, 0x18, 0xD4, 0x91, 0x8D, 0xB0, 0xD9, 0x1F, 0xD8, 0x0C, 0xEF, 0x08, 0xDF, 0xF8, 0x21, 0x0D, 
    0xE2, 0x21, 0x0B, 0xAE, 0xDF, 0xF6, 0x27, 0x08, 0x00, 0x5A, 0x8E, 0xDA, 0x8C, 0x7F, 0x5A, 0xDF, 
    0xF4, 0x21, 0x0E, 0xDE, 0x21, 0x0A, 0xAE, 0xDF, 0xF2, 0x24, 0x10, 0xDE, 0xD8, 0x00, 0xD6, 0x08, 
    0x78, 0x26, 0x08, 0x00, 0x5E, 0xE4, 0x04, 0x04, 0x25, 0xDF, 0xF0, 0xD6, 0x1E, 0x3C, 0xF3, 0x1E, 
    0x7F, 0xE4, 0x00, 0x1E, 0x18, 0xFF, 0xD0, 0x72, 0xD4, 0x75, 0x48, 0x90, 0xD9, 0x1C, 0xD8, 0x18, 
    0xEF, 0x04, 0xD1, 0xC0, 0x5C, 0xF2, 0x18, 0x24, 0x10, 0x7F, 0x60, 0x24, 0x08, 0xE0, 0x24, 0x18, 
    0xB8, 0x24, 0x10, 0xE0, 0x24, 0x08, 0xE0, 0xD2, 0x47, 0xD9, 0x1C, 0xD8, 0x18, 0x1C, 0x18, 0xE0, 
    0xD2, 0x48, 0xD9, 0x1C, 0xD8, 0x18, 0x28, 0x10, 0xE0, 0x28, 0x08, 0xE0, 0x28, 0x18, 0xB8, 0xDF, 
    0xFF, 0x28, 0x10, 0xE0, 0x28, 0x08, 0xE0, 0xD1, 0xC0, 0x22, 0xDF, 0xFE, 0x28, 0x18, 0xB8, 0xDF, 
    0xFC, 0x26, 0x10, 0xE0, 0x26, 0x08, 0xE0, 0xDF, 0xFA, 0x26, 0x18, 0xB8, 0xDF, 0xF8, 0x26, 0x10, 
    0xE0, 0x26, 0x08, 0xE0, 0xDF, 0xF6, 0x2A, 0x18, 0xE0, 0xDF, 0xF4, 0x8E, 0xE2, 0xDF, 0xF2, 0x89, 
    0xE0, 0xD8, 0x00, 0xDF, 0xF0, 0x1F, 0x1E, 0xE0, 0xFF, 0xD0, 0x72, 0xD4, 0xE9, 0x34, 0xC0, 0xD9, 
    0x1C, 0xD8, 0x18, 0xEF, 0x03, 0xD1, 0xC0, 0x5C, 0xDF, 0xF4, 0x15, 0x60, 0x7F, 0x60, 0xE4, 0x18, 
    0x48, 0x07, 0xDF, 0x00, 0xF2, 0x18, 0x07, 0x0F, 0xE0, 0x07, 0x09, 0xB0, 0x09, 0x0F, 0xE0, 0x09, 
    0x09, 0xB0, 0xDF, 0xFF, 0x07, 0x0F, 0xE0, 0x07, 0x09, 0xB0, 0xD1, 0xC0, 0x22, 0xDF, 0xF2, 0x15, 
    0x48, 0xE0, 0xE4, 0x18, 0x30, 0x0B, 0xDF, 0x00, 0xD4, 0x00, 0x40, 0xC0, 0xD9, 0x1F, 0xD8, 0x00, 
    0xEF, 0x00, 0xD6, 0x7E, 0xD2, 0x18, 0x7E, 0xE4, 0xE4, 0x00, 0x7E, 0x00, 0xFF, 0xD0, 0x72, 0xD5, 
    0x8C, 0xDD, 0xE0, 0xD9, 0x1F, 0xD8, 0x1B, 0xEF, 0x06, 0xDF, 0xFB, 0xD1, 0xC0, 0x5C, 0xDF, 0xFD, 
    0x67, 0x7F, 0x18, 0x7F, 0x54, 0x67, 0x80, 0x10, 0xE4, 0xDF, 0x00, 0x67, 0x82, 0x08, 0xDA, 0xDF, 
    0x01, 0x67, 0x81, 0x10, 0xDA, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xFF, 0x67, 0x81, 0x10, 0xDA, 0xDF, 
    0x00, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0x01, 0x67, 0x81, 0x10, 0xDA, 0xDF, 0xFF, 0x67, 0x82, 0x08, 
    0xDA, 0xDF, 0xFB, 0x67, 0x80, 0x10, 0xE8, 0xDF, 0xFF, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xFD, 0x67, 
    0x80, 0x10, 0xE5, 0xDF, 0x01, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xFA, 0x67, 0x7F, 0x18, 0xD4, 0xD1, 
    0xC0, 0x22, 0xDF, 0xFB, 0xCF, 0x7F, 0xD4, 0xDF, 0xF9, 0x67, 0x80, 0x10, 0xE4, 0xDF, 0xFC, 0x67, 
    0x82, 0x08, 0xDA, 0xDF, 0xFB, 0x67, 0x81, 0x10, 0xDA, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xF7, 0x67, 
    0x81, 0x10, 0xDA, 0xDF, 0xFC, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xF7, 0x67, 0x81, 0x10, 0xDA, 0xDF, 
    0xF5, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xEF, 0x67, 0x80, 0x10, 0xE8, 0xDF, 0xF3, 0x67, 0x82, 0x08, 
    0xDA, 0xDF, 0xEF, 0x67, 0x80, 0x10, 0xE5, 0xDF, 0xF3, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xF0, 0x67, 
    0x7F, 0x1E, 0xD4, 0xFF, 0xD0, 0x72, 0xD5, 0x6A, 0xDD, 0xA6, 0xD9, 0x1F, 0xD8, 0x1B, 0xEF, 0x06, 
    0xDF, 0xFB, 0xD1, 0xC0, 0x5C, 0xF2, 0x02, 0xDF, 0xFD, 0x67, 0x7F, 0x18, 0x7F, 0x54, 0x67, 0x80, 
    0x10, 0xE4, 0xDF, 0x00, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0x01, 0x67, 0x81, 0x10, 0xDA, 0x67, 0x82, 
    0x08, 0xDA, 0xDF, 0xFF, 0x67, 0x81, 0x10, 0xDA, 0xDF, 0x00, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0x01, 
    0x67, 0x81, 0x10, 0xDA, 0xDF, 0xFF, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xFB, 0x67, 0x80, 0x10, 0xE8, 
    0xDF, 0xFF, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xFD, 0x67, 0x80, 0x10, 0xE5, 0xDF, 0x01, 0x67, 0x82, 
    0x08, 0xDA, 0xDF, 0xFA, 0x67, 0x7F, 0x18, 0xD4, 0xD1, 0xC0, 0x22, 0xDF, 0xFB, 0xCF, 0x7F, 0xD4, 
    0xDF, 0xF9, 0x67, 0x80, 0x10, 0xE4, 0xDF, 0xFC, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xFB, 0x67, 0x81, 
    0x10, 0xDA, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xF7, 0x67, 0x81, 0x10, 0xDA, 0xDF, 0xFC, 0x67, 0x82, 
    0x08, 0xDA, 0xDF, 0xF7, 0x67, 0x81, 0x10, 0xDA, 0xDF, 0xF5, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xEF, 
    0x67, 0x80, 0x10, 0xE8, 0xDF, 0xF3, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xEF, 0x67, 0x80, 0x10, 0xE5, 
    0xDF, 0xF3, 0x67, 0x82, 0x08, 0xDA, 0xDF, 0xF0, 0x67, 0x7F, 0x1E, 0xD4, 0xFF, 
};

u8 seq_tracks = 8;

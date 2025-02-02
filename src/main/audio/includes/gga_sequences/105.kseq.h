u8 seq_data[] =
{
    0x00, 0x10, 0x01, 0x2F, 0x01, 0xDC, 0x02, 0x74, 0x03, 0x0C, 0x03, 0x80, 0x03, 0xED, 0x96, 0x0D, 
    0xDA, 0xB3, 0x78, 0xEF, 0x0A, 0xD0, 0x86, 0xD4, 0x3C, 0x56, 0xC9, 0xD7, 0x73, 0x00, 0xD8, 0x28, 
    0xE0, 0x06, 0xE1, 0x24, 0x32, 0x2D, 0xF2, 0x14, 0xF2, 0x30, 0x2D, 0x22, 0x7F, 0x64, 0xE4, 0x00, 
    0x0A, 0x2F, 0x2D, 0x0E, 0xE4, 0x2F, 0x51, 0x7A, 0x64, 0x32, 0x0F, 0x7F, 0x62, 0x34, 0x0A, 0x00, 
    0x64, 0x34, 0x18, 0x26, 0x50, 0x34, 0x0E, 0x7F, 0x64, 0x32, 0x23, 0xE4, 0x34, 0x0D, 0xE4, 0x2F, 
    0x30, 0x3A, 0x64, 0x2B, 0x1C, 0x7F, 0x5F, 0xE4, 0x1C, 0x16, 0x2D, 0xE0, 0x00, 0xEF, 0x05, 0xD5, 
    0xCD, 0xDD, 0x40, 0x67, 0x83, 0x22, 0xA3, 0x67, 0x83, 0x0E, 0xA3, 0x67, 0x84, 0x22, 0xA3, 0x67, 
    0x83, 0x0E, 0xA3, 0x67, 0x83, 0x22, 0xA3, 0x67, 0x83, 0x0E, 0xA3, 0x67, 0x84, 0x22, 0xA3, 0x67, 
    0x83, 0x0E, 0xA3, 0x67, 0x86, 0x22, 0xD0, 0x67, 0x83, 0x0E, 0xA3, 0x67, 0x85, 0x22, 0xD0, 0x67, 
    0x85, 0x0E, 0xD0, 0x67, 0x83, 0x22, 0xA3, 0x67, 0x83, 0x0E, 0xA3, 0x67, 0x84, 0x22, 0xA3, 0x67, 
    0x83, 0x0E, 0xA3, 0xEF, 0x0A, 0xD4, 0x3C, 0x56, 0xC9, 0xD7, 0x73, 0x00, 0xD8, 0x28, 0xE0, 0x06, 
    0xF2, 0x14, 0xF2, 0x30, 0x2D, 0x22, 0xE4, 0xE4, 0x00, 0x0A, 0x2F, 0x2D, 0x0E, 0xE4, 0x2F, 0x51, 
    0x7A, 0x64, 0x32, 0x0F, 0x7F, 0x62, 0x34, 0x0A, 0x00, 0x64, 0x34, 0x18, 0x66, 0x50, 0x37, 0x0E, 
    0x7F, 0x64, 0x34, 0x23, 0xE4, 0x32, 0x0D, 0xE4, 0x2F, 0x30, 0x3A, 0x64, 0x2B, 0x1C, 0x7F, 0x5F, 
    0xE4, 0x1C, 0x16, 0x2D, 0xE0, 0x00, 0xEF, 0x05, 0xD5, 0xCD, 0xDD, 0x40, 0x67, 0x83, 0x22, 0xA3, 
    0x67, 0x83, 0x0E, 0xA3, 0x67, 0x84, 0x22, 0xA3, 0x67, 0x83, 0x0E, 0xA3, 0x67, 0x83, 0x22, 0xA3, 
    0x67, 0x83, 0x0E, 0xA3, 0x67, 0x84, 0x22, 0xA3, 0x67, 0x83, 0x0E, 0xA3, 0x67, 0x86, 0x22, 0xD0, 
    0x67, 0x83, 0x0E, 0xA3, 0x67, 0x85, 0x22, 0xD0, 0x67, 0x85, 0x0E, 0xD0, 0x67, 0x83, 0x22, 0xA3, 
    0x67, 0x83, 0x0E, 0xA3, 0x67, 0x84, 0x22, 0xA3, 0x67, 0x83, 0x0E, 0xA3, 0xF1, 0x00, 0xFF, 0xEF, 
    0x00, 0xD0, 0x86, 0xD5, 0xB9, 0xDD, 0x40, 0x60, 0x30, 0x7F, 0x41, 0x61, 0x22, 0xC1, 0x60, 0x0E, 
    0xC1, 0xF2, 0x22, 0xC8, 0xC1, 0x61, 0x22, 0xC1, 0x60, 0x0E, 0xC1, 0x60, 0x22, 0xC1, 0x60, 0x0E, 
    0xC1, 0x61, 0x22, 0xC1, 0x60, 0x0E, 0xC1, 0x60, 0x30, 0xC1, 0xC8, 0xC1, 0xC5, 0xC3, 0x67, 0x73, 
    0x22, 0xD5, 0x67, 0x73, 0x0E, 0xD5, 0xF2, 0x22, 0xC5, 0xC3, 0x67, 0x73, 0x22, 0xD5, 0x5D, 0x0E, 
    0xC3, 0x5F, 0x30, 0xC3, 0x67, 0x73, 0x22, 0xD5, 0x67, 0x73, 0x0E, 0xD5, 0xF2, 0x22, 0xC6, 0xC6, 
    0x67, 0x73, 0x22, 0xD5, 0x5E, 0x0E, 0xC6, 0xDD, 0x40, 0x60, 0x30, 0xC1, 0x61, 0x22, 0xC1, 0x60, 
    0x0E, 0xC1, 0xF2, 0x22, 0xC8, 0xC1, 0x61, 0x22, 0xC1, 0x60, 0x0E, 0xC1, 0x60, 0x22, 0xC1, 0x60, 
    0x0E, 0xC1, 0x61, 0x22, 0xC1, 0x60, 0x0E, 0xC1, 0x60, 0x30, 0xC1, 0xC8, 0xC1, 0xC5, 0xC3, 0x67, 
    0x73, 0x22, 0xD5, 0x67, 0x73, 0x0E, 0xD5, 0xF2, 0x22, 0xC5, 0xC3, 0x67, 0x73, 0x22, 0xD5, 0x5D, 
    0x0E, 0xC3, 0x5F, 0x30, 0xC3, 0x67, 0x73, 0x22, 0xD5, 0x67, 0x73, 0x0E, 0xD5, 0xF2, 0x22, 0x5E, 
    0x07, 0xC1, 0xC6, 0xBC, 0x67, 0x73, 0x22, 0xD5, 0x5E, 0x0E, 0xCB, 0xFF, 0xEF, 0x00, 0xD0, 0x86, 
    0xD4, 0xB4, 0x32, 0xC0, 0x07, 0x30, 0x7F, 0x64, 0x07, 0x22, 0xB2, 0x02, 0x0E, 0xDA, 0x07, 0x30, 
    0xE4, 0x07, 0x22, 0xB2, 0x0B, 0x0E, 0xDA, 0x0C, 0x30, 0xE4, 0x0C, 0x22, 0xB2, 0x07, 0x0E, 0xDA, 
    0x0C, 0x22, 0xE4, 0xE4, 0x18, 0x14, 0x07, 0x07, 0x0E, 0xDA, 0x0B, 0x30, 0xE4, 0x71, 0x73, 0x64, 
    0x71, 0x7F, 0x28, 0x71, 0x94, 0xF2, 0x30, 0xF2, 0x30, 0xF2, 0x22, 0x09, 0x0E, 0xE4, 0x0E, 0x22, 
    0xE4, 0x0E, 0x0E, 0xE4, 0x09, 0x22, 0xE4, 0x0E, 0x0E, 0xE4, 0xD2, 0x32, 0xDD, 0xC0, 0x07, 0x30, 
    0xE4, 0x07, 0x22, 0xB2, 0x02, 0x0E, 0xDA, 0x07, 0x30, 0xE4, 0x07, 0x22, 0xB2, 0x0B, 0x0E, 0xDA, 
    0x0C, 0x30, 0xE4, 0x0C, 0x22, 0xB2, 0x07, 0x0E, 0xDA, 0x0C, 0x22, 0xE4, 0xE4, 0x18, 0x14, 0x07, 
    0x07, 0x0E, 0xDA, 0x0B, 0x30, 0xE4, 0x71, 0x73, 0x64, 0x71, 0x7F, 0x28, 0x71, 0x94, 0xF2, 0x30, 
    0xF2, 0x30, 0xF2, 0x22, 0x09, 0x0E, 0xE4, 0x02, 0x22, 0xE4, 0x02, 0x0E, 0xE4, 0x04, 0x22, 0xE4, 
    0x06, 0x0E, 0xE4, 0xFF, 0xEF, 0x05, 0xD0, 0x86, 0xD4, 0x6E, 0x56, 0xB9, 0xD7, 0x7F, 0x00, 0xD8, 
    0x28, 0xE1, 0x24, 0x32, 0x2D, 0xF2, 0x30, 0x2D, 0x22, 0x7F, 0x64, 0xE4, 0x00, 0x0A, 0x2F, 0x2D, 
    0x0E, 0xE4, 0x2F, 0x51, 0x7A, 0x64, 0x32, 0x0F, 0x7F, 0x62, 0x34, 0x0A, 0x00, 0x64, 0x34, 0x18, 
    0x26, 0x50, 0x34, 0x0E, 0x7F, 0x64, 0x32, 0x23, 0xE4, 0x34, 0x0D, 0xE4, 0x2F, 0x30, 0x3A, 0x64, 
    0x93, 0x7F, 0x5F, 0xE4, 0x1C, 0x16, 0x2D, 0xEF, 0x0A, 0x95, 0x60, 0x64, 0x95, 0x7F, 0x14, 0xF2, 
    0x60, 0xF2, 0xC0, 0xEF, 0x05, 0xD2, 0x56, 0xDD, 0xB9, 0xD7, 0x7F, 0x00, 0xD8, 0x28, 0xF2, 0x30, 
    0x2D, 0x22, 0xE4, 0xE4, 0x00, 0x0A, 0x2F, 0x2D, 0x0E, 0xE4, 0x2F, 0x51, 0x7A, 0x64, 0x32, 0x0F, 
    0x7F, 0x62, 0x34, 0x0A, 0x00, 0x64, 0x34, 0x18, 0x66, 0x50, 0x37, 0x0E, 0x7F, 0x64, 0x34, 0x23, 
    0xE4, 0x32, 0x0D, 0xE4, 0x2F, 0x30, 0x3A, 0x64, 0x93, 0x7F, 0x5F, 0xE4, 0x1C, 0x16, 0x2D, 0xEF, 
    0x0A, 0x95, 0x60, 0x64, 0x95, 0x7F, 0x14, 0xF2, 0x60, 0xF2, 0xC0, 0xFF, 0xEF, 0x0F, 0xD0, 0x86, 
    0xD4, 0x57, 0x56, 0x90, 0x23, 0x30, 0x7F, 0x32, 0x23, 0x22, 0x33, 0x64, 0x23, 0x3E, 0x7F, 0x32, 
    0x23, 0x22, 0x33, 0x64, 0x23, 0x0E, 0x00, 0x32, 0x24, 0x30, 0x7F, 0x32, 0x24, 0x22, 0xE4, 0x24, 
    0x0E, 0xE4, 0x24, 0x30, 0xB2, 0x8B, 0xE4, 0x8C, 0x40, 0x64, 0xF2, 0x90, 0xF2, 0x60, 0xE1, 0x24, 
    0x32, 0x2D, 0x24, 0x22, 0x66, 0x55, 0x24, 0x3E, 0x7F, 0x5A, 0xD2, 0x56, 0xDD, 0x90, 0x23, 0x30, 
    0xB2, 0x23, 0x22, 0x33, 0x64, 0x23, 0x3E, 0x7F, 0x32, 0x23, 0x22, 0x33, 0x64, 0x23, 0x0E, 0x00, 
    0x32, 0x24, 0x30, 0x7F, 0x32, 0x24, 0x22, 0xE4, 0x24, 0x0E, 0xE4, 0x24, 0x30, 0xB2, 0x8B, 0xE4, 
    0x8C, 0x40, 0x64, 0xF2, 0x90, 0xF2, 0x60, 0x24, 0x22, 0x60, 0x5A, 0x24, 0x3E, 0x7F, 0x5F, 0xFF, 
    0xEF, 0x0F, 0xD0, 0x86, 0xD4, 0x57, 0x56, 0xD9, 0xF2, 0x01, 0x26, 0x30, 0x7F, 0x32, 0x26, 0x22, 
    0x33, 0x64, 0x26, 0x3E, 0x7F, 0x32, 0x26, 0x22, 0x33, 0x64, 0x26, 0x0E, 0x00, 0x32, 0x28, 0x30, 
    0x7F, 0x32, 0x28, 0x22, 0xE4, 0x28, 0x0E, 0xE4, 0x28, 0x30, 0xB2, 0x8E, 0xE4, 0x90, 0x40, 0x64, 
    0xF2, 0x90, 0xF2, 0x60, 0xE1, 0x24, 0x32, 0x2D, 0x2A, 0x22, 0x66, 0x55, 0x2A, 0x3D, 0x7F, 0x5A, 
    0xFA, 0x07, 0xFB, 0x06, 0xFF, 0xD2, 0x56, 0xDD, 0xD9, 0xF2, 0x01, 0x26, 0x30, 0xB2, 0x26, 0x22, 
    0x33, 0x64, 0x26, 0x3E, 0x7F, 0x32, 0x26, 0x22, 0x33, 0x64, 0x26, 0x0E, 0x00, 0x32, 0x28, 0x30, 
    0x7F, 0x32, 0x28, 0x22, 0xE4, 0x28, 0x0E, 0xE4, 0x28, 0x30, 0xB2, 0x8E, 0xE4, 0x90, 0x40, 0x64, 
    0xF2, 0x90, 0xF2, 0x60, 0x2A, 0x22, 0x60, 0x5A, 0x2A, 0x3D, 0x7F, 0x5F, 0xFF, 
};

u8 seq_tracks = 8;

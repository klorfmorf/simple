OUTPUT_ARCH (mips)

/* Starts a segment */
#define BEGIN_SEG(name, addr) \
    _##name##SegmentStart = ADDR(.name); \
    _##name##SegmentRomStart = __romPos; \
    .name addr : AT(__romPos)

/* Ends a segment */
#define END_SEG(name) \
    _##name##SegmentEnd = ALIGN(ADDR(.name) + SIZEOF(.name),0x10); \
    _##name##SegmentRomEnd = _##name##SegmentEnd - _##name##SegmentStart + _##name##SegmentRomStart; \
    __romPos = _##name##SegmentRomEnd; \
    . = _##name##SegmentEnd;

/* Starts a noload segment which is intended to contain variables which do not have data yet. */
#define BEGIN_NOLOAD(name) \
    _##name##SegmentBssStart = ADDR(.name.noload); \
    .name.noload (NOLOAD) :

/* Ends a noload segment */
#define END_NOLOAD(name) \
    _##name##SegmentBssEnd = ALIGN(ADDR(.name.noload) + SIZEOF(.name.noload),0x10); \
    _##name##SegmentBssSize = _##name##SegmentBssEnd - _##name##SegmentBssStart; \
    . = _##name##SegmentBssEnd;

SECTIONS
{
   __romPos = 0;

   BEGIN_SEG(boot, 0x04000000)
   {
      BUILD_DIR/asm/rom_header.o(.text); /* ROM Header */
      BUILD_DIR/boot.6102.o(.data); /* CIC 6102 bootcode */
   }
   END_SEG(boot)

   BEGIN_SEG(code, 0x80000400) SUBALIGN(16)
   {
		BUILD_DIR/asm/entry.o(.start);
      BUILD_DIR/src/main*.o(.text);

      */libultra.a:*.o(.text);
      */libgcc.a:*.o(.text);
      /usr/lib/n64/PR/rspboot.o(.text);
      /usr/lib/n64/PR/gspF3DEX.fifo.o(.text);
      /usr/lib/n64/PR/aspMain.o(.text);

      /* data */
      BUILD_DIR/src/main*.o(.data*);

      */libultra.a:*.o(.data*);
      /usr/lib/n64/PR/rspboot.o(.data*);
      /usr/lib/n64/PR/gspF3DEX.fifo.o(.data*);
      /usr/lib/n64/PR/aspMain.o(.data*);

      /* rodata */
      BUILD_DIR/src/main*.o(.*rodata*);

      */libultra.a:*.o(.*rodata*);
      */libgcc.a:*.o(.*rodata*);
   }
   END_SEG(code)
   BEGIN_NOLOAD(code)
   {
      BUILD_DIR/src/main*.o(.*bss*);
      BUILD_DIR/src/buffers/yield_data.o(.*bss*);
      BUILD_DIR/src/buffers/output_buffer.o(.*bss*);
      BUILD_DIR/src/buffers/dram_stack.o(.*bss*);

      */libultra.a:*.o(COMMON);
      */libultra.a:*.o(.scommon);
      */libultra.a:*.o(.*bss*);
   }
   END_NOLOAD(code)

   BEGIN_SEG(sound_data, __romPos)
   {
      BUILD_DIR/assets/sound_data.o(.data*);
   }
   END_SEG(sound_data)

   BEGIN_SEG(ascii_font, 0x05000000)
   {
      BUILD_DIR/src/ascii_font*.o(.text);
      BUILD_DIR/src/ascii_font*.o(.data);
      BUILD_DIR/src/ascii_font*.o(.rodata);
   }
   END_SEG(ascii_font)

   . = 0x80261000;
   
   BEGIN_NOLOAD(graphics_buffers)
   {
      BUILD_DIR/src/buffers/frame_buffer.o(.bss*);
      BUILD_DIR/src/buffers/depth_buffer.o(.bss*);
   }
   END_NOLOAD(graphics_buffers)

   BEGIN_NOLOAD(miscellaneous_buffers)
   {
      BUILD_DIR/src/buffers/memory_allocation_buffer.o(.bss*);
      BUILD_DIR/src/buffers/ascii_font_buffer.o(.bss*);
   }
   END_NOLOAD(miscellaneous_buffers)

   . = 0x80304000;

   BEGIN_NOLOAD(overlay_buffer)
   {
      BUILD_DIR/src/buffers/overlay_buffer.o(.bss*);
   }
   END_NOLOAD(overlay_buffer)

   _RomSize = __romPos;
   
   /* Discard everything not specifically mentioned above. */
   /DISCARD/ :
   {
      *(*);
   }
}

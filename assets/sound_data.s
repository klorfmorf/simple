.include "macros.inc"

.section .data

glabel _midibankSegmentRomStart
.incbin "assets/Bank00.mnsg.ctl.bin"
.balign 16
glabel _midibankSegmentRomEnd

glabel _miditableSegmentRomStart
.incbin "assets/Bank00.mnsg.tbl.bin"
.balign 16
glabel _miditableSegmentRomEnd

glabel _midibank2SegmentRomStart
.incbin "assets/Bank01.mnsg.ctl.bin"
.balign 16
glabel _midibank2SegmentRomEnd

glabel _miditable2SegmentRomStart
.incbin "assets/Bank01.mnsg.tbl.bin"
.balign 16
glabel _miditable2SegmentRomEnd

glabel _midibank3SegmentRomStart
.incbin "assets/Bank02.mnsg.ctl.bin"
.balign 16
glabel _midibank3SegmentRomEnd

glabel _miditable3SegmentRomStart
.incbin "assets/Bank02.mnsg.tbl.bin"
.balign 16
glabel _miditable3SegmentRomEnd

glabel _seqSegmentRomStart
.balign 16
glabel _seqSegmentRomEnd
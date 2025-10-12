set architecture i8086
layout asm
layout reg
set disassembly-flavor intel
target remote localhost:26000
layout split
b *0x7c00

set architecture i386:x86-64
layout asm
layout reg
set disassembly-flavor intel
target remote localhost:26000
layout split
catch exception
continue
b *0x7c00

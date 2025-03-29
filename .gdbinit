define hook-stop
    printf "[%4x:%4x] ", $cs, $eip
end

set architecture i8086
layout asm
layout reg
set disassembly-flavor intel
target remote localhost:26000
symbol-file build/os/os
b *0x7c00
b main

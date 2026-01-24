org 0x8000

[bits 16]
start:
	jmp real_mode

gdt32_start:
	dd 0x00000000
	dd 0x00000000
	dd 0x0000FFFF
	dd 0x00CF9A00
	dd 0x0000FFFF
	dd 0x00CF9200
gdt32_end:

align 4
gdt32_location:
	dw gdt32_end - gdt32_start - 1
	dd gdt32_start

gdt64_start:
	dq 0x0000000000000000
	dq 0x00209A0000000000
	dq 0x0000920000000000
gdt64_end:

align 4
gdt64_location:
	dw gdt64_end - gdt64_start - 1
	dd gdt64_start

align 4096
PML4:
	dq PDPT + 0x03
	times 511 dq 0

align 4096
PDPT:
	dq PD + 0x03
	times 511 dq 0

align 4096
PD:
	dq 0x00000000 + 0x83
	dq 0x00200000 + 0x83
	times 510 dq 0

real_mode:
	cli

        lgdt [gdt32_location]

        in al, 0x92
        or al, 0x02
        out 0x92, al

        mov eax, cr0
        or eax, 0x1
        mov cr0, eax

        jmp 0x08:protected_mode

[bits 32]
protected_mode:
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        mov esp, 0x90000

	lgdt [gdt64_location]

	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	mov eax, PML4
	mov cr3, eax

	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	jmp 0x08:long_mode

[bits 64]
long_mode:
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
	mov rsp, 0x90000

	mov rax, 0x00030000
	jmp rax

org 0x0010000

[bits 16]
start:
	mov ax, 1
	hlt

	jmp $

        lgdt [gdt_location]

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
	jmp 0x00200000

	hlt


gdt_start:
	dq 0x0000000000000000
	dq 0x00209A0000000000
	dq 0x0000920000000000
gdt_end:

align 4
gdt_location:
	dw gdt_end - gdt_start - 1
	dd gdt_start + 0x00100000

align 4096
PML4:
	dq PDPT + 0x03

align 4096
PDPT:
	dq PD + 0x03

align 4096
PD:
	dq 0x00100000 + 0x83
	dq 0x00300000 + 0x83

dw 0xDEADBEEF

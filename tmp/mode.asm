org 0x0010000

[bits 16]
start:
	mov ax, 1
	mov bx, 1
	mov cx, 1

	hlt
	jmp $

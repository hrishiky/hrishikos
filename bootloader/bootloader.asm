org 0x7c00

[bits 16]

start:
	jmp boot

load_msg db "bootloader: loaded", 0ah, 0dh, 0h
diskerr_msg db "bootloader: disk read failed", 0ah, 0dh, 0h
jmperr_msg db "bootloader: kernel jump failed", 0ah, 0dh, 0h

CursorX db 0
CursorY db 0

boot:
	cli
	cld

	call clear_screen
	mov si, load_msg
	call print

	mov ax, 0x1000
 	mov es, ax
	xor bx, bx

	mov al, 17
	mov ch, 0
	mov cl, 2
	mov dh, 0
	mov dl, 0
	mov ah, 0x02
	int 0x13

	jc disk_error

	mov ax, 0x1000
	mov es, ax
	mov bx, 17 * 512

	mov ah, 0x02
	mov al, 8
	mov ch, 0
	mov cl, 1
	mov dh, 1
	mov dl, 0
	int 0x13

	jc disk_error

	; mov ax, 0x3000
 	; mov es, ax
	; xor bx, bx

	; mov ah, 0x02
	; mov al, 31
	; mov ch, 0
	; mov cl, 9
	; mov dh, 1
	; mov dl, 0
	; int 0x13

	; jc disk_error

	jmp 0x1000:0x0000

disk_error:
	mov si, diskerr_msg
	call print

	hlt


clear_screen:
	mov ah, 06h
	mov al, 0
	mov bh, 07h
	mov cx, 0
	mov dx, 184fh
	int 10h

	mov byte [CursorX], 0
	mov byte [CursorY], 0
	call mov_cursor

	ret

mov_cursor:
	mov ah, 02h
	mov bh, 0
	mov dl, [CursorX]
	mov dh, [CursorY]
	int 10h

	ret

put_char:
	mov ah, 0eh
	mov bh, 0
	int 10h
	inc byte [CursorX]
	call mov_cursor

	ret

print:
.loop:
	lodsb
	or al, al
	jz .done
	call put_char
	jmp .loop

.done:
	ret

times 510 - ($ - $$) db 0
dw 0xAA55

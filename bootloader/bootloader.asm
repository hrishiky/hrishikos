org 0x7c00

[bits 16]

start:
	jmp boot

load_msg db "bootloader: loaded", 0ah, 0dh, 0h
diskerr_msg db "bootloader: disk read failed", 0ah, 0dh, 0h

CursorX db 0
CursorY db 0

BootDrive db 0

DAP:
	db 0x10
	db 0x00
count:	dw 33
offset:	dw 0x0000
sgmnt:	dw 0x0800
lba:	dq 1

boot:
	cli
	cld

	mov [BootDrive], dl

	call clear_screen
	mov si, load_msg
	call print

	mov ah, 0x42
	mov si, DAP
	mov dl, [BootDrive]
	int 0x13
	jc disk_error

	mov word [count], 5
	mov word [offset], 0x0000
	mov word [sgmnt], 0x3000
	mov word [lba], 34

	mov ah, 0x42
	mov si, DAP
	mov dl, [BootDrive]
	int 0x13
	jc disk_error

	%if 0
	mov ax, 0x800
 	mov es, ax
	xor bx, bx
	mov ah, 0x02
	mov al, 17
	mov ch, 0
	mov cl, 2
	mov dh, 0
	mov dl, [BootDrive]
	int 0x13
	jc disk_error

	mov ax, 0x800
	mov es, ax
	mov bx, 17 * 512
	mov ah, 0x02
	mov al, 16
	mov ch, 0
	mov cl, 1
	mov dh, 1
	mov dl, [BootDrive]
	int 0x13
	jc disk_error

	mov ax, 0x3000
 	mov es, ax
	xor bx, bx
	mov ah, 0x02
	mov al, 2 ; old: 3
	mov ch, 0
	mov cl, 17
	mov dh, 1
	mov dl, [BootDrive]
	int 0x13
	jc disk_error
	%endif 0

	jmp 0x0:0x8000

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

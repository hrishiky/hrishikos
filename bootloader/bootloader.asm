org 0x7c00
bits 16
start: jmp boot

msg db "Welcome to Hrishik OS!", 0ah, 0dh, 0h

CursorX db 0
CursorY db 0


boot:
	cli
	cld

	call ClrScr
	mov si, msg
	call Print

	mov ax, 50h
	mov es, ax
	xor bx, bx

	mov al, 2
	mov ch, 0
	mov cl, 2
	mov dh, 0
	mov dl, 0
	mov ah, 02h
	int 13h
	jmp 50h:0h

	hlt

ClrScr:
	mov ah, 06h
	mov al, 0
	mov bh, 07h
	mov cx, 0
	mov dx, 184fh
	int 10h

MovCursor:
        mov ah, 02h
        mov bh, 0
	mov dl, [CursorX]
	mov dh, [CursorY]
        int 10h

        ret

PutChar:
        mov ah, 0eh
        mov bh, 0
        int 10h

        inc byte [CursorX]
        call MovCursor

        ret

Print:
.loop:
        lodsb
        or al, al
        jz .done
        call PutChar
        jmp .loop

.done:
        ret

times 510 - ($-$$) db 0
dw 0xAA55

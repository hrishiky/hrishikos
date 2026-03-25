#include "interrupt_frame.h"
#include "stdio.h"
#include "keyboard.h"
#include "asm_wrappers.h"
#include "irq.h"

void int_irq_central_handler(Int_Regs* regs, unsigned long vector, unsigned long error) {
	switch (vector) {
		case INT_IRQ_VECTOR_TIMER:
			vga_text_print("timer");
			outb(0x20, 0x20);
			break;

		case INT_IRQ_VECTOR_KEYBOARD:
			unsigned char data = inb(KEYBOARD_PORT_DATA);
			keyboard_central_handler(data);
			outb(KEYBOARD_EOI, KEYBOARD_PIC1_PORT_COMMAND);
			break;

		default:
			printf("IRQ INTERRUPT: 0x%x\n", vector);
		        printf("ERROR: 0x%x\n", error);
			outb(0x20, 0x20);
			break;
	}
}

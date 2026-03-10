#include "interrupt_frame.h"
#include "vga_text.h"
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
			vga_text_print("unhandled irq interrupt.\n");
			vga_text_print("IRQ INTERRUPT ENCOUNTERED: ");
		        vga_text_print_hex((long)&vector);
		        vga_text_print("\nERROR: ");
		        vga_text_print_hex((long)&error);
		        vga_text_print("\n");

			outb(0x20, 0x20);
			break;
	}
}

#include "interrupt_frame.h"
#include "vga_text.h"
#include "keyboard.h"
#include "irq.h"

void irq_outb(unsigned char data, unsigned short port) {
        __asm__ volatile (
                "outb %0, %1"
                :
                : "a"(data), "d"(port)
        );
}


unsigned char irq_inb(unsigned short port) {
        unsigned char data;

        __asm__ volatile (
                "inb %1, %0"
                : "=a"(data)
                : "d"(port)
        );

        return data;

}

void int_irq_central_handler(Int_Regs* regs, unsigned long vector, unsigned long error) {
	switch (vector) {
		case INT_IRQ_VECTOR_TIMER:
			vga_text_print("timer");
			irq_outb(0x20, 0x20);
			break;

		case INT_IRQ_VECTOR_KEYBOARD:
			unsigned char data = irq_inb(KEYBOARD_PORT_DATA);
			keyboard_central_handler(data);
			irq_outb(KEYBOARD_EOI, KEYBOARD_PIC1_PORT_COMMAND);
			break;

		default:
			vga_text_print("unhandled irq interrupt.\n");
			vga_text_print("IRQ INTERRUPT ENCOUNTERED: ");
		        vga_text_print_hex((char*)&vector, 8, 16);
		        vga_text_print("\nERROR: ");
		        vga_text_print_hex((char*)&error, 8, 16);
		        vga_text_print("\n");

			irq_outb(0x20, 0x20);
			break;
	}
}

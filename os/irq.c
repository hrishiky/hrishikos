#include "interrupt_frame.h"
#include "vga_text.h"
#include "irq.h"

void int_irq_central_handler(Int_Regs* regs) {
	vga_text_print("IRQ interrupt encountered");
}

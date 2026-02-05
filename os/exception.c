#include "interrupt_frame.h"
#include "vga_text.h"
#include "exception.h"

void int_exception_central_handler(Int_Regs* regs, unsigned long vector, unsigned long error) {
	vga_text_print("CPU EXCEPTION!");
}

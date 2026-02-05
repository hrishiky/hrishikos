#include "interrupt.h"
#include "interrupt_frame.h"
#include "exception.h"
#include "irq.h"

void int_central_handler(Int_Regs* regs) {
	// __asm__ volatile ("mov $0x6941, %rax");
	// __asm__ volatile ("hlt");

	return;

	unsigned long vector = *(unsigned long*)((unsigned char*)regs + ISR_VECTOR_NUMBER_OFFSET);
	unsigned long error = *(unsigned long*)((unsigned char*)regs + ISR_ERROR_CODE_OFFSET);

	if (vector < 32) {
		int_exception_central_handler(regs, vector, error);
		return;
	} else if (vector < 48) {
		int_irq_central_handler(regs);
		return;
	} else {
		return;
	}
}

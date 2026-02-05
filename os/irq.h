#ifndef IRQ_H
#define IRQ_H

#include "interrupt_frame.h"
#include "vga_text.h"

void int_irq_central_handler(Int_Regs* regs);

#endif

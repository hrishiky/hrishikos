#ifndef IRQ_H
#define IRQ_H

#include "keyboard.h"
#include "interrupt_frame.h"
#include "vga_text.h"

#define INT_IRQ_VECTOR_TIMER 32
#define INT_IRQ_VECTOR_KEYBOARD 33

void int_irq_central_handler(Int_Regs* regs, unsigned long vector, unsigned long error);

#endif

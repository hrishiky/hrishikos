#include "idt.h"
#include "vga_text.h"

__attribute__((aligned(0x10)))
Idt_Entry idt[IDT_ENTRY_COUNT];
Idtr idtr;

void idt_idt_zero_fill(void) {
	for (unsigned short i = 0; i < IDT_ENTRY_COUNT; i++) {
		idt[i].isr_low = 0;
		idt[i].kernel_cs = 0;
		idt[i].ist = 0;
		idt[i].attributes = 0;
		idt[i].isr_mid = 0;
		idt[i].isr_high = 0;
		idt[i].reserved = 0;
	}
}

void idt_idtr_zero_fill(void) {
	idtr.limit = 0;
	idtr.base = 0;
}

void idt_idt_entry_fill(unsigned short index, void (*stub)(void)) {
	unsigned long address = (unsigned long)stub;

	idt[index].isr_low = address & 0xFFFF;
	idt[index].kernel_cs = KERNEL_CS;
	idt[index].ist = 0;
	idt[index].attributes = IDT_ATTRIBUTE_INTERRUPT_GATE;
	idt[index].isr_mid = (address >> 16) & 0xFFFF;
	idt[index].isr_high = 0;
	idt[index].reserved = 0;
}

void idt_idt_fill(void) {
	#define X(n) idt_idt_entry_fill(n, isr##n);
	INT_CPU_LIST
	#undef X

	#define X(n) idt_idt_entry_fill(32 + n, irq##n);
	INT_IRQ_LIST
	#undef X

	#define X(n) idt_idt_entry_fill(n, isr##n);
	INT_REMAINING_LIST
	#undef X
}

void idt_idtr_fill(void) {
	idtr.limit = IDT_LIMIT;
	idtr.base = (unsigned long)idt;
}

void idt_load_idtr(void) {
	__asm__ volatile ("lidt %0"
		:
		: "m"(idtr)
		:
	);

	//__asm__ volatile ("hlt");
	 __asm__ volatile ("sti");
}

void idt_init(void){
	idt_idt_zero_fill();
	idt_idtr_zero_fill();

	idt_idt_fill();
	idt_idtr_fill();

	idt_load_idtr();

	vga_text_print(IDT_LOAD_SUCCESS_MESSAGE);
}

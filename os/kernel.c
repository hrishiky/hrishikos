#include "idt.h"
#include "shell.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"

void main(void* boot_info) {
	idt_init();

	pmm_init(boot_info);
	vmm_init();
	heap_init();

	shell_main();

	__asm__("hlt");
}

#include "idt.h"
#include "shell.h"
#include "pmm.h"

void main(void* boot_info) {
	idt_init();

	pmm_init(boot_info);

	shell_main();

	__asm__("hlt");
}

void main() {
	__asm__ volatile (
		"mov $64, %ax"
	);

	char* vga = (char*)0xB8000;

	vga[0] = 'A';
	vga[1] = 0x0F;
}

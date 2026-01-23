void main() {
	char* vga = (char*)0x000B8000;

	vga[0] = 'H';
	vga[1] = 0x0F;
	__asm__("hlt");
}

ENTRY(main)

SECTIONS {
	. = 0x30000;
	.text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss) }
}

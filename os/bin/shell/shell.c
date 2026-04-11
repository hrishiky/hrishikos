#include "keyboard.h"
#include "shell.h"
#include "vga_text.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "asm_wrappers.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"

extern unsigned char vga_text_cursor_x;
extern unsigned char vga_text_cursor_y;

extern Boot_Info* pmm_boot_info;
extern unsigned int pmm_memory_size;
extern unsigned char kernel_end;

Shell_Command commands[] = {
	{ "help", &shell_command_help },
	{ "echo", &shell_command_echo },
	{ "clear", &shell_command_clear },
	{ "reboot", &shell_command_reboot },
	{ "color", &shell_command_color },
	{ "halt", &shell_command_halt },
	{ "pmminfo", &shell_command_pmminfo }
};

unsigned short shell_command_history_start = 0;
Shell_Arguments shell_command_history[SHELL_COMMAND_HISTORY_COUNT];

unsigned char shell_foreground_color = VGA_TEXT_COLOR_WHITE;
unsigned char shell_background_color = VGA_TEXT_COLOR_BLACK;

unsigned char shell_input_start_x;
unsigned char shell_input_start_y;

void shell_print_prompt(void) {
	printf("$ ");

	shell_input_start_x = vga_text_cursor_x;
	shell_input_start_y = vga_text_cursor_y;
}

void shell_print_spacing(void) {
	if (vga_text_cursor_x == 0) {
		if (vga_text_cursor_y == 0) {
			return;
		} else {
			printf("\n");
		}
	} else {
		printf("\n\n");
	}
}

char* shell_get_input(void) {
	static char input[SHELL_COMMAND_LENGTH_MAXIMUM];
	memset(input, 0, SHELL_COMMAND_LENGTH_MAXIMUM);

	unsigned short index = 0;

	while (1) {
		if (index == SHELL_COMMAND_LENGTH_MAXIMUM - 1) {
			input[index] = '\0';
			return input;
		}

		char character = keyboard_buffer_read();

		if (character == -1) {
			continue;
		}

		if (character == '\n') {
			vga_text_print_character('\n');
			input[index] = '\0';
			return input;
		}

		if (character == '\b') {
			if (vga_text_cursor_x == shell_input_start_x &&
			    vga_text_cursor_y == shell_input_start_y) {
				continue;
			}

			if (index > 0) {
				index--;
				input[index] = '\0';
				vga_text_print_character('\b');
				continue;
			}
		}

		vga_text_print_character(character);
		input[index++] = character;
		input[index] = '\0';
	}
}

Shell_Arguments shell_input_parse(char* input) {
	Shell_Arguments arguments;
	arguments.argc = 0;

	for (unsigned short i = 0; i < SHELL_ARGC_MAXIMUM; i++) {
		arguments.argv[i][0] = '\0';
	}

	unsigned short argv_index = 0;
	unsigned long i = 0;

	while (input[i] == SHELL_COMMAND_DELIMITER) {
		i++;
	}

	for (; i < strlen(input); i++) {
		if (input[i] == SHELL_COMMAND_DELIMITER) {
			while (input[i + 1] == SHELL_COMMAND_DELIMITER) {
				i++;
			}

			if (argv_index > 0) {
				arguments.argv[arguments.argc][argv_index] = '\0';
				arguments.argc++;
				argv_index = 0;

				if (arguments.argc >= SHELL_ARGC_MAXIMUM) {
					return arguments;
				}
			}

			continue;
		}

		if (argv_index < SHELL_ARGV_MAXIMUM - 1) {
			arguments.argv[arguments.argc][argv_index++] = input[i];
			arguments.argv[arguments.argc][argv_index] = '\0';
		}
	}

	if (argv_index > 0) {
		arguments.argv[arguments.argc][argv_index] = '\0';
		arguments.argc++;
	}

	return arguments;
}


void shell_command_history_add(Shell_Arguments arguments) {
	if (shell_command_history_start >= SHELL_COMMAND_HISTORY_COUNT) {
		shell_command_history_start -= SHELL_COMMAND_HISTORY_COUNT;
	}

	shell_command_history[shell_command_history_start++] = arguments;
}

void shell_run_command(Shell_Arguments arguments) {
	if (strcmp(arguments.argv[0], "\0")) {
		return;
	}

	for (unsigned short i = 0; i < SHELL_COMMANDS_COUNT; i++) {
		if (strcmp(commands[i].command, arguments.argv[0])) {
			shell_command_history_add(arguments);
			commands[i].function_pointer(arguments);
			return;
		}
	}

	printf("invalid command '%s'\n", arguments.argv[0]);
	printf("use 'help' to see valid commands");
}

void shell_main(void) {
	vga_text_clear_screen();
	vga_text_blinking_cursor_enable(0, 15);

	while (1) {
		shell_print_prompt();
		char* input = shell_get_input();
		Shell_Arguments arguments = shell_input_parse(input);
		shell_run_command(arguments);
		shell_print_spacing();
	}
}

void shell_command_help(Shell_Arguments arguments) {
	printf("commands:\n");

	for (unsigned short i = 0; i < SHELL_COMMANDS_COUNT; i++) {
		printf("  %s\n", commands[i].command);
	}
}

void shell_command_echo(Shell_Arguments arguments) {
	for (int i = 1; i < arguments.argc; i++) {
		printf("%s ", arguments.argv[i]);
	}
}

void shell_command_clear(Shell_Arguments arguments) {
	vga_text_clear_screen();
	vga_text_cursor_shift(0, 0);
}

void shell_command_reboot(Shell_Arguments arguments) {
	__asm__ volatile (
		"lidt (0)"
		:
		:
		: "memory"
	);

	__asm__ volatile ("int $3");
}

void shell_command_color(Shell_Arguments arguments) {
	if (arguments.argc != 3) {
		vga_text_print("usage: color [FOREGROUND_COLOR] [BACKGROUND_COLOR]\n");

		return;
	}

	vga_text_change_colors(strtol(arguments.argv[1], 0), strtol(arguments.argv[2], 0));
	vga_text_refresh_colors();
}

void shell_command_halt(Shell_Arguments arguments) {
	while (inb(0x64) & 1) {
		inb(0x60);
	}

	halt();
}

void shell_command_pmminfo(Shell_Arguments arguments) {
	E820_Entry* memory_map = (E820_Entry*) pmm_boot_info->entries;

	printf("total memory size: %d KiB\n", pmm_memory_size / 8192);
	printf("pmm bitmap start address: 0x%x \n\n", (unsigned long) &kernel_end);
	printf("physical memory map:\n");

	for (unsigned char i = 0; i < pmm_boot_info->entry_count; i++) {
		printf("  region %d: base = 0x%x; length = 0x%x; type = %d\n", i + 1, memory_map[i].base, memory_map[i].length, memory_map[i].type);
	}

	void* ptr = heap_alloc(4096);

	char* c_ptr = (char*) ptr;
	c_ptr[0] = 'H';
	c_ptr[1] = 'i';
	c_ptr[2] = '!';
	c_ptr[3] = '\0';

	printf((char*) ptr);
}

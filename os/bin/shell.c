#include "keyboard.h"
#include "shell.h"
#include "vga_text.h"
#include "string.h"
#include "sys_io.h"
#include "mem.h"

extern unsigned char vga_text_cursor_x;
extern unsigned char vga_text_cursor_y;

Shell_Command commands[] = {
	{ "help", &shell_command_help },
	{ "echo", &shell_command_echo },
	{ "clear", &shell_command_clear },
	{ "reboot", &shell_command_reboot },
	{ "color", &shell_command_color }
};

unsigned short shell_command_history_start = 0;
Shell_Arguments shell_command_history[SHELL_COMMAND_HISTORY_COUNT];

unsigned char shell_foreground_color = VGA_TEXT_COLOR_WHITE;
unsigned char shell_background_color = VGA_TEXT_COLOR_BLACK;

unsigned char shell_input_start_x;
unsigned char shell_input_start_y;

void shell_print_prompt(void) {
	vga_text_print("$ ");

	shell_input_start_x = vga_text_cursor_x;
	shell_input_start_y = vga_text_cursor_y;
}

void shell_print_spacing(void) {
	if (vga_text_cursor_x != 0) {
		vga_text_print("\n\n");
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

	for (unsigned long i = 0; i < strlen(input); i++) {
		if (input[i] == SHELL_COMMAND_DELIMITER) {
			if (arguments.argc >= SHELL_ARGC_MAXIMUM) {
				return arguments;
			}

			arguments.argc++;
			argv_index = 0;
			continue;
		}

		if (argv_index >= SHELL_ARGV_MAXIMUM) {
			continue;
		}

		arguments.argv[arguments.argc][argv_index++] = input[i];
		arguments.argv[arguments.argc][argv_index] = '\0';
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
	for (unsigned short i = 0; i < SHELL_COMMANDS_COUNT; i++) {
		if (strcmp(commands[i].command, arguments.argv[0]) == 0) {
			shell_command_history_add(arguments);
			commands[i].function_pointer(arguments);
			return;
		}
	}

	vga_text_print("invalid command\nuse 'help' to see valid commands");
}

void shell_main(void) {
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
	vga_text_print("currently implemented commands:\n");

	for (unsigned short i = 0; i < SHELL_COMMANDS_COUNT; i++) {
		vga_text_print(commands[i].command);
		vga_text_print_character('\n');
	}
}

void shell_command_echo(Shell_Arguments arguments) {
	for (int i = 1; i < arguments.argc; i++) {
		vga_text_print(arguments.argv[i]);
		vga_text_print_character(' ');
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
	//

	vga_text_change_colors(VGA_TEXT_COLOR_WHITE, VGA_TEXT_COLOR_BLUE);
	vga_text_refresh_colors();
}

#include "shell.h"

#include "stdio.h"

extern Shell_Command commands[];

void shell_command_help(Shell_Arguments arguments) {
	printf("commands:\n");

	for (unsigned short i = 0; i < SHELL_COMMANDS_COUNT; i++) {
		printf("\t%s\n", commands[i].command);
	}
}

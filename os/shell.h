#ifndef SHELL_H
#define SHELL_H

#include "keyboard.h"
#include "vga_text.h"
#include "string.h"

#define SHELL_COMMANDS_COUNT 2
#define SHELL_COMMAND_LENGTH_MAXIMUM 1024
#define SHELL_COMMAND_DELIMITER ' '
#define SHELL_ARGC_MAXIMUM 64
#define SHELL_ARGV_MAXIMUM 128

typedef struct {
        int argc;
        char argv[SHELL_ARGC_MAXIMUM][SHELL_ARGV_MAXIMUM];
} Shell_Arguments;

typedef struct {
        char* command;
        void (*function_pointer)(int, char[SHELL_ARGC_MAXIMUM][SHELL_ARGV_MAXIMUM]);
} Shell_Command;

void shell_print_prompt(void);
void shell_print_spacing(void);
char* shell_get_input(void);
Shell_Arguments shell_input_parse(char* input);
void shell_run_command(int argc, char argv[SHELL_ARGC_MAXIMUM][SHELL_ARGV_MAXIMUM]);
void shell_main(void);

void shell_command_help(int argc, char argv[SHELL_ARGC_MAXIMUM][SHELL_ARGV_MAXIMUM]);
void shell_command_echo(int argc, char argv[SHELL_ARGC_MAXIMUM][SHELL_ARGV_MAXIMUM]);

#endif

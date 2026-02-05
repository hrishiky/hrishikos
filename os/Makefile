BUILD_DIR = ../build/os
DRIVER_DIR = ../build/drivers

OS = $(BUILD_DIR)/os

CFLAGS += -ffreestanding -nostdlib -gdwarf-4 -m64 -ggdb3 -I../drivers

OS_C_SRCS := $(wildcard *.c)
OS_ASM_SRCS := $(wildcard *.asm)

OS_C_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_C_SRCS))
OS_ASM_OBJS := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(OS_ASM_SRCS))

OS_OBJS := $(OS_C_OBJS) $(OS_ASM_OBJS)

DRIVER_OBJS := $(wildcard $(DRIVER_DIR)/*.o)

all: $(OS)

$(BUILD_DIR)/%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.asm
	nasm -f elf64 $< -o $@

$(OS): $(OS_OBJS)
	ld -m elf_x86_64 -nmagic -T os.lds $(OS_OBJS) $(DRIVER_OBJS) -o $@

clean:
	rm -f $(OS_OBJS)

BUILD_DIR=../build/os
OS=$(BUILD_DIR)/os

CFLAGS+=-ffreestanding -nostdlib -gdwarf-4 -m64 -ggdb3

OS_SRCS := $(wildcard *.c)
OS_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_SRCS))

all: $(OS)

$(BUILD_DIR)/%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

$(OS): $(OS_OBJS)
	# ld -m elf_x86_64 -nmagic -T os.lds $(OS_OBJS) -o $@
	ld -m elf_x86_64 -T os2.lds -o $(OS).elf $(OS_OBJS)
	objcopy -O binary $(OS).elf $(OS)

clean:
	rm $(OS_OBJS)

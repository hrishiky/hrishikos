BUILD_DIR = ../build/drivers

DRIVER_SRCS := $(wildcard *.c)
DRIVER_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(DRIVER_SRCS))

CFLAGS += -ffreestanding -nostdlib -gdwarf-4 -m64 -ggdb3


all: $(DRIVER_OBJS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm $(BUILD_DIR)/*

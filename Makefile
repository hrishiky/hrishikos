BUILD_DIR=build
BOOTLOADER=$(BUILD_DIR)/bootloader/bootloader.o
MODE=$(BUILD_DIR)/bootloader/mode.o
OS=$(BUILD_DIR)/os/os
DISK_IMG=$(BUILD_DIR)/disk.img

BOOTLOADER_SRCS := $(wildcard *.asm)
BOOTLOADER_OBJS := $(patsubst %.asm, %.o, $(BOOTLOADER_SRCS))

all: bootdisk

.PHONY: bootdisk bootloader os

bootloader:
	make -C bootloader

os:
	make -C os

bootdisk: bootloader os
	dd if=/dev/zero of=$(DISK_IMG) bs=512 count=2880
	dd conv=notrunc if=$(BOOTLOADER) of=$(DISK_IMG) bs=512 count=1 seek=0

	# dd conv=notrunc if=$(MODE) of=$(DISK_IMG) bs=512 count=25 seek=1
	dd conv=notrunc if=$(MODE) of=$(DISK_IMG) bs=512 count=1 seek=1

	# dd conv=notrunc if=$(OS) of=$(DISK_IMG) bs=512 count=$$(($(shell stat --printf="%s" $(OS))/512)) seek=26
	dd conv=notrunc if=$(OS) of=$(DISK_IMG) bs=512 count=$$(($(shell stat --printf="%s" $(OS))/512)) seek=2

qemu:
	qemu-system-i386 -machine q35 -fda $(DISK_IMG) -gdb tcp::26000 -S

qemu-no-debug:
	qemu-system-i386 -machine q35 -fda $(DISK_IMG)

clean:
	make -C bootloader clean
	make -C os clean
	rm $(DISK_IMG)

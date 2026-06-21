# Variables for compilers and flags
ASM = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy
QEMU = qemu-system-i386

# Default variant if none is provided
VARIANT ?= base

# Output directory
BUILD_DIR = bin

# Targets that should always run
.PHONY: all run clean

all: $(BUILD_DIR)/os-disk-$(VARIANT).bin

$(BUILD_DIR)/os-disk-$(VARIANT).bin: $(BUILD_DIR)/boot-sect0.bin $(BUILD_DIR)/kernel-$(VARIANT).bin
	@mkdir -p $(BUILD_DIR)
	# Concatenate the bootsector and the kernel into a single raw OS image disk
	cat $^ > $@

$(BUILD_DIR)/boot-sect0.bin: boot-sect0.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) $< -f bin -o $@

$(BUILD_DIR)/kernel-$(VARIANT).bin: $(BUILD_DIR)/call-kernel.o $(BUILD_DIR)/kernel-$(VARIANT).o $(BUILD_DIR)/idt.o 
	# Link the entry assembly file and the C kernel together at address 0x1000
	$(LD) -m elf_i386 -o $(BUILD_DIR)/kernel-$(VARIANT).elf -Ttext 0x1000 $^
	# Extract raw binary instructions out of the ELF file
	$(OBJCOPY) -O binary $(BUILD_DIR)/kernel-$(VARIANT).elf $@

$(BUILD_DIR)/call-kernel.o: call-kernel.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) $< -f elf32 -o $@

$(BUILD_DIR)/idt.o: idt.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) $< -f elf32 -o $@

$(BUILD_DIR)/kernel-$(VARIANT).o: kernel-$(VARIANT).c
	@mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -c $< -o $@

run: all
	$(QEMU) -drive format=raw,file=$(BUILD_DIR)/os-disk-$(VARIANT).bin

clean:
	rm -f $(BUILD_DIR)/*.bin $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf
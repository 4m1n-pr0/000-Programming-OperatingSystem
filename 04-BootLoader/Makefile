# Variables for compilers and flags
ASM = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy
QEMU = qemu-system-i386

# Output directory
BUILD_DIR = bin

# Targets
all: $(BUILD_DIR)/os-disk.bin

$(BUILD_DIR)/os-disk.bin: $(BUILD_DIR)/boot-sect0.bin $(BUILD_DIR)/kernel.bin
	@mkdir -p $(BUILD_DIR)
	# Concatenate the bootsector and the kernel into a single raw OS image disk
	cat $(BUILD_DIR)/boot-sect0.bin $(BUILD_DIR)/kernel.bin > $(BUILD_DIR)/os-disk.bin

$(BUILD_DIR)/boot-sect0.bin: boot-sect0.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) boot-sect0.asm -f bin -o $(BUILD_DIR)/boot-sect0.bin

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/call-kernel.o $(BUILD_DIR)/kernel.o
	# Link the entry assembly file and the C kernel together at address 0x1000
	$(LD) -m elf_i386 -o $(BUILD_DIR)/kernel.elf -Ttext 0x1000 $(BUILD_DIR)/call-kernel.o $(BUILD_DIR)/kernel.o
	# Extract raw binary instructions out of the ELF file
	$(OBJCOPY) -O binary $(BUILD_DIR)/kernel.elf $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/call-kernel.o: call-kernel.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) call-kernel.asm -f elf32 -o $(BUILD_DIR)/call-kernel.o

$(BUILD_DIR)/kernel.o: kernel.c
	@mkdir -p $(BUILD_DIR)
	# -m32 compiles for 32-bit x86, -ffreestanding ensures no standard C library is used
	$(CC) -m32 -ffreestanding -c kernel.c -o $(BUILD_DIR)/kernel.o

run: all
	$(QEMU) -drive format=raw,file=$(BUILD_DIR)/os-disk.bin

clean:
	rm -f $(BUILD_DIR)/*.bin $(BUILD_DIR)/*.o
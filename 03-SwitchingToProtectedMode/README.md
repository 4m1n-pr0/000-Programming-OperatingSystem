# 03-SwitchingToProtectedMode

## Theory

The file `boot-sect0.asm` is a minimal x86 boot sector designed to transition the CPU from **16-bit Real Mode** to **32-bit Protected Mode**.

When an x86 computer first turns on, it starts in a legacy compatibility state (16-bit). This code acts as the bridge to unlock modern 32-bit hardware capabilities.

#### Real Mode (16-bit) and Protected Mode (32-bit)

To understand what this code achieves, it helps to look at the two environments it straddles:

- **Real Mode (16-bit):** The default state of the CPU upon boot. It can only access up to 1 MB of RAM, does not support memory protection (any program can overwrite any part of memory, including the OS), and uses basic 16-bit registers.
- **Protected Mode (32-bit):** Unlocks the full potential of modern processors. It allows the CPU to access up to 4 GB of RAM, enables hardware-level memory protection (preventing apps from crashing each other), and allows the use of 32-bit extended registers (like `eax`, `ebp`, `esp`).

> Note: Protected mode is possible on `16-bit`. However, it is best to transition to protected mode and `32-bit` at the "same" time.

#### What is the Global Descriptor Table (GDT)?

Before the CPU can safely enter Protected Mode, it requires a blueprint of how memory should be handled. This is where the **Global Descriptor Table (GDT)** comes in.

The Global Descriptor Table (GDT) is like a master registry or a database of memory segments that the CPU uses to understand how memory is carved up and who has permission to access it.
Instead of letting programs access any random part of the RAM, the CPU looks at the GDT to see the "rules" for different memory blocks. It tells the processor: `This chunk of memory is for code, that chunk is for data, this one is for the operating system, and that one is for regular user apps.`

In the current boot sector, a simple "Flat Memory Model" GDT is defined at the bottom (`GDT_BEGIN` to `GDT_END`). It sets up two overlapping 4 GB segments—one for executable code (`GDT_CODE_SEG`) and one for data (`GDT_DATA_SEG`)—giving the CPU full access to the system memory.

#### Code Breakdown

First, the following line will precise the memory address the boot sector will be loaded to.

```asm
[org 0x7c00]
```

Second, the boot sector executes the mode switch through a precise sequence of steps.

```asm
cli
lgdt [GDT_DESC]
```

- `cli` (Clear Interrupt Flag): Disables hardware interrupts. The 16-bit BIOS interrupt handlers will no longer work in 32-bit mode, so it is turned off to prevent the system from crashing.
- `lgdt` (Load GDT): Tells the CPU where the GDT structure lives in memory using the `GDT_DESC` pointer. Loads it into the GDT Register.

Next, the switch to protected mode will be done here.
```asm
mov eax , cr0
or  eax , 0x1
mov cr0 , eax
```

- The CPU has a special control register called `cr0`. By setting its very first bit (Bit 0, the Protection Enable bit) to `1`, we officially tell the CPU hardware to enter Protected Mode.

After, the following instruction will jump directly to the 32-bit code.

```asm
jmp  CODE_SEG:INIT_PM
```

- Modern CPUs fetch and pre-cache several instructions ahead to speed up execution. Because we just changed the CPU modes, any 16-bit instructions already sitting in the CPU pipeline cache are now invalid.
- Performing a **Far Jump** (`jmp` to a different segment) forces the CPU to discard its pre-fetched cache, reload the segment registers using the new GDT rules, and start fresh in 32-bit mode.

The final step is initializing 32-bit Mode.

```asm
[bits 32]
INIT_PM:
mov ax , DATA_SEG
mov ds , ax ... ; Updates segment registers
mov ebp , 0x90000
mov esp , ebp   ; Updates Stack Pointer
```

- `[bits 32]`: Tells the assembler (NASM) to start compiling instructions into 32-bit machine code from this point forward.
- The data segment registers (`ds`, `ss`, `es`, etc.) are updated to point to the newly defined `DATA_SEG` from the GDT and establish a safe position in memory for the stack (`esp`).

> Note: Please look at the code for additional information as comments.

## Practice

To compile the boot sector, execute this command : 

```bash
nasm boot-sect0.asm -f bin -o boot.bin
```

- `-f bin`: Format of the produced file as a flat raw binary (MS-DOS, embedded, ...) [default]
- `-o boot.bin`: Output file (bootable)

To boot on the compiled file, execute this command : 
```bash
qemu-system-i386 -drive format=raw,file=boot.bin
```

- `-drive`: This flag defines a new block device (like a hard disk, CD-ROM, or floppy) for the virtual machine.

- `format=raw`: This specifies that the disk image does not have a complex structure (like .qcow2 or .vmdk). It is a "raw" byte-for-byte image of the disk.

- `file=boot.bin`: This points to the actual file on the host machine that QEMU will treat as the primary storage device (the "virtual hard drive" or "floppy").
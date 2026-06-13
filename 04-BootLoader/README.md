# 04-BootLoader

## Theory

#### Boot Loader

A Boot Loader is an executable code that will load the kernel of an operating system into memory (RAM), then call its entrypoint function.

In this case, the Boot Loader is sitting directly into the MBR.

```asm
mov bx , 0x1000 ; Memory offset to which kernel will be loaded
mov ah , 0x02   ; Bios Read Sector Function
mov al , 30     ; No. of sectors to read
mov ch , 0x00   ; Select Cylinder 0 from hard disk
mov dh , 0x00   ; Select head 0 from hard disk
mov cl , 0x02   ; Start Reading from Second sector(Sector just after boot sector)

int 0x13        ; Bios Interrupt Relating to Disk functions
```

One final step is to actually call the kernel caller. Calling to the hard coded `0x1000` address will execute the instructions from the file `call-kernel.asm`. Therefore, the start function from the kernel.c file will be exeuted and a `K` character will printed. 

```asm
call 0x1000 ; The kernel was loaded to the 0x1000 address, so the call points where the kernel binary executable code is residing
```

#### Kernel

For the moment, the kernel only prints a character to the screen.

When QEMU runs, the BIOS initializes the video card into a legacy compatibility state called VGA Mode 3 (Video Graphics Array) text mode, with no GUI (Graphical User Interface) and only a TUI (Text User Interface).

In this mode, the video card's graphics processing units are essentially asleep. Instead of expecting the OS to send a massive matrix of color pixels (like a 1920x1080 desktop background), the video card expects a very small, simple array of text characters. This specific legacy hardware standard dictates a grid size of exactly 80 columns by 25 rows (giving you 2,000 total character slots on screen). Each character on screen will take two bytes (character + colors). This makes a total of 4,000 bytes (80×25×2=4000 bytes).

The video card itself contains a built-in "font ROM" (a hardware look-up table) that knows exactly which pixels to light up on the physical monitor to draw an 'A', a 'B', or a '7'.

Why the kernel prints the character `K` to the address `0xb8000` ? This process is called Memory-Mapped Input/Output (MMIO). The motherboard is wired so that physical RAM addresses starting at 0xb8000 don't actually point to your system memory sticks; instead, they route directly to the memory inside the VGA video card. Thus, the card will now see a `K` a,d print it to the address `0xb8000` which is the top-left of the screen.

#### Makefile

To make things more efficient than building every time with a lot of commands the whole operating system from scratch, a Makefile is used. It is a file taken as an input to the `make` command. It contains instructions to build an executable binary. Look at this [guide](https://makefiletutorial.com/) for further information.

At the end, a file `bin/os-disk.bin` will be built. It represents the disk that contains the boot sector and the kernel of the OS, on which QEMU will boot.

First, the `kernel.c` will be compiled with this command :
```bash
gcc -m32 -ffreestanding -c kernel.c -o bin/kernel.o
```

- `-m32`: Forces GCC to compile for a 32-bit x86 architecture. Without this flag and on a 64 bits computer, GCC would generate 64-bit instructions, which the 32-bit Protected Mode CPU would instantly crash on.
- `-ffreestanding` : This tells GCC that there is no standard library (no stdio.h, no printf, no memory management). A normal C program relies on Linux to print text or allocate memory. Here, this is a custom OS, so GCC must not assume any underlying helper functions exist.
- `-c` : Compiles the code into an object file (.o) but does not link it yet. It just generates the raw machine code for the functions.


Second, the `call-kernel.asm ` will be assembled with this command :
```bash
nasm call-kernel.asm -f elf32 -o bin/call-kernel.o
```

This takes the human-readable C code and translates it into machine code (`kernel.o`).
- `-f elf32` : Specifies the ouput format as 32-bit ELF (Executable and Linkable Format).
- `-o` : Output file


Third, the linker will take the two object files and link them into a binary file with this command :

```bash
ld -m elf_i386 -o bin/kernel.elf -Ttext 0x1000 bin/call-kernel.o bin/kernel.o
```

- `-m elf_i386` : Tells the linker to output a 32-bit x86 ELF binary.
- `-Ttext 0x1000` : This is incredibly crucial. It tells the linker: "Assume that when this code is actually running in real life, it will be sitting at memory address 0x1000." The linker uses this to calculate the exact memory locations for internal function calls and variable lookups. If this number is changed without changing the bootloader's target address, the kernel will look for data in the wrong place and crash.


Fourth, the `kernel.elf` must be stripped of its ELF format to transorm it into a raw binary. As ELF is only understandable on Linux distributions, it cannot work on this OS. This command will do :
```bash
objcopy -O binary bin/kernel.elf bin/kernel.bin
```
- `-O binary` : This tells objcopy to strip away all the ELF headers, metadata, and debugging symbols. It extracts only the raw, naked binary instructions and spits them out into kernel.bin. What is left is pure machine code ready to execute sequentially.


Finally, the last step is to concatenate the bootsector and the kernel into a single raw OS image disk with the following command :
```bash
cat bin/boot-sect0.bin bin/kernel.bin > bin/os-disk.bin
```

> Note: Why is it needed to have the `elf32` binary format as an intermediary step ? If there was only raw binary since the beginning, `ld` would not be able to link the kernel and its caller. Thus, the `elf32` is used for linking and `objcopy` will strip the headers and metadata in order to transorm the `kernel.elf` into a pure binary raw file.

#### Putting it all together

Here is the workflow that makes `QEMU` boot on the built OS :
- `BIOS` : Sitting on a computer's EEPROM (or a file in the case of `QEMU`), it will execute the POST, setting up I/O devices...
- `MBR` : The `boot-sect0`, exactly 512 bytes will be executed (contains: Boot Loader, GDT, Switch to 32 bits, Call the kernel caller).
- `Boot Loader` : Load the Kernel and its caller into memory at the address `0x1000` (`ld` linked the kernel and its caller at this address in memory).
- `GDT` : Sets up the GDT, which assigns segments of RAM for the OS, regular apps and so on.
- `Protected Mode`: Switch from 16 to 32-bit mode, enabling memory protection and access to 32-bit registers.
- `Call Kernel`: Calls the `start` function from the kernel.
- `Kernel` : Prints the character `K` on screen by using its `start` function.

## Practice

To build the whole operating system, execute this command : 
```bash
make
```

To boot on the operating system, execute this command :
```bash
make run
```

To clean the `bin` directory, execute this command :
```bash
make clean
```
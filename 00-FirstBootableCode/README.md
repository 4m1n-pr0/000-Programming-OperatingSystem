# 00-FirstBootableCode

## Theory

This code represents a basic boot sector, also known as the Master Boot Record (MBR). It consists of exactly 512 bytes located at the very beginning of a bootable drive. During startup, the BIOS searches for this specific sector, verifies it by looking for the 'magic number' at the end, and loads it into memory for the CPU to execute.

The first loop will hang the CPU so it does not execute anything after (and does not crash).


```asm
loop: // Defines a section in memory
    jmp loop // Jump to the 'loop' memory address => infinit loop
```

Then, as the boot sector must consist of exactly 512 bytes, the following instruction will fill the compiled file with exactly 510 bytes, so 2 bytes remain for the magic number.

```asm
times 510-($-$$) db 0
```

- `$`: Represents the current memory address where the assembler is currently writing.

- `$$`: Represents the starting memory address of the current section.

- `($ - $$)`: This calculates the total size of your code in bytes so far.

- `510 - ($ - $$)`: This calculates exactly how many bytes of empty space are left before reaching the 510th byte.


Finally, this instruction add exactly two bytes to represent the famous magic number.

```asm
dw 0xaa55
```

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

- `file=boot.bin`: This points to the actual file on your host machine that QEMU will treat as the primary storage device (the "virtual hard drive" or "floppy").

---

> Note : For having a proof that the `boot.bin` is a bootable code, execute `file boot.bin`. It should output something like `boot.bin: DOS/MBR boot sector`
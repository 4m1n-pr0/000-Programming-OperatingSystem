# 01-HelloWorld

## Theory

The file `boot-sect0.asm` represents a basic boot sector. It will print the string of characters `HELLO WORLD` to the screen.

The first line of code will move `0x0e` (The Scrolling Teletype), to `ah`. Hence, the BIOS will know the cursor must move to the next position on the screen after printing a new character, thus called the scrolling teletype.

```asm
mov ah, 0x0e
```

After that, the code will mostly repeat itself for printing the string of characters `HELLO WORLD` to the screen. The character to be printed must be in the `al` register (because the BIOS will search at this location). Next, the code trigger an interrupt with the code `0x10` which will make the CPU jumping into a location where there are BIOS display routines for printing on the screen.

```asm
mov al , '?' // In the actual code, '?' is replaced by the characters of the string "HELLO WORLD" one after the other. 
int 0x10
```

For the CPU to hang and not crash, an infinit loop is implemented.

```asm
jmp $ // This exact line of code will make the CPU jumping to this location "forever"
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

> Note : The computer boots on Real-Mode (16 bits). Therefore, it is not possible to access 32 bit registers like eax or ebx. The code only deal with 16 bit registers like ah or al.
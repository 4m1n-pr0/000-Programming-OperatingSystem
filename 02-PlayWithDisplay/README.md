# 02-PlayWithDisplay

## Theory

The files `boot-sect0-color.asm` and `boot-sect0-infinite.asm` represent basic boot sectors. The first will print the string of characters `HELLO WORLD` to the screen with colors. The second will print "forever" the character `H` on the screen.

As much of the code was explained in previous steps, the theory section will focus on what is new.

In the `boot-sect0-color.asm`, some code was added to set the colors on screen.

```asm
mov ah , 0x0b // Tell the BIOS the function to be called is 'Set Color Palette'
mov bh , 0x0 // Tell the BIOS to choose the sub-function which changes the background color
mov bl , 0xff // Selects the color
int 0x10
```

In the `boot-sect0-infinite.asm`, some code was added to set an infinite loop.

```asm
infinite:
    mov al , 'H' // Selects the character to print on screen
    int 0x10 // Trigger an interupt
    jmp infinite // Jumps back to the memory section infinite => loops "forever"
```

## Practice

To compile the boot sectors, execute these commands: 

```bash
nasm boot-sect0-infinite.asm -f bin -o boot-i.bin
nasm boot-sect0-color.asm -f bin -o boot-c.bin
```

- `-f bin`: Format of the produced file as a flat raw binary (MS-DOS, embedded, ...) [default]
- `-o *.bin`: Output file (bootable)

To boot on the compiled file, execute this command : 
```bash
qemu-system-i386 -drive format=raw,file=boot-i.bin
qemu-system-i386 -drive format=raw,file=boot-c.bin
```

- `-drive`: This flag defines a new block device (like a hard disk, CD-ROM, or floppy) for the virtual machine.

- `format=raw`: This specifies that the disk image does not have a complex structure (like .qcow2 or .vmdk). It is a "raw" byte-for-byte image of the disk.

- `file=*.bin`: This points to the actual file on your host machine that QEMU will treat as the primary storage device (the "virtual hard drive" or "floppy").

---

> Note : The computer boots on Real-Mode (16 bits). Therefore, it is not possible to access 32 bit registers like eax or ebx. The code only deal with 16 bit registers like ah or al.
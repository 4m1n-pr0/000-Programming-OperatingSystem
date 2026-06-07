[org 0x7c00]    ; Tells the assembler where the BIOS loads this code in memory (origin address)

[bits 16]       ; Tell assembler to emit 16-bit architectural instructions (Real Mode default)

; --- SWITCH TO PROTECTED MODE ---
cli             ; Disable CPU interrupts; safe mode switch requires no background interruptions
lgdt [GDT_DESC] ; Load the GDT descriptor register with the table's size and location

mov eax , cr0   ; Read Control Register 0 (contains CPU operation mode flags) into EAX
or  eax , 0x1   ; Set bit 0 (PE - Protection Enable) to 1, keeping all other bits unchanged
mov cr0 , eax   ; Write back to CR0; the hardware is now officially in Protected Mode

jmp  CODE_SEG:INIT_PM ; Perform a far jump to the code segment; flushes the 16-bit CPU pipeline cache

[bits 32]       ; Tell assembler to emit 32-bit architectural instructions from this point forward

INIT_PM:
mov ax , DATA_SEG ; Load the GDT data segment selector offset into AX
mov ds , ax     ; Update Data Segment register to use the 32-bit GDT descriptor
mov ss , ax     ; Update Stack Segment register to use the 32-bit GDT descriptor
mov es , ax     ; Update Extra Segment register to use the 32-bit GDT descriptor
mov fs , ax     ; Update F Segment register to use the 32-bit GDT descriptor
mov gs , ax     ; Update G Segment register to use the 32-bit GDT descriptor

mov ebp , 0x90000 ; Set the base pointer to a safe, free area deep in memory
mov esp , ebp     ; Set the stack pointer to match the base pointer, initializing the stack

jmp $           ; Infinite loop ("Hang") to halt code execution here indefinitely

; --- GLOBAL DESCRIPTOR TABLE (GDT) DEFINITION ---

GDT_BEGIN:      ; Label marking the start of the GDT structure

GDT_NULL_DESC:  ; The mandatory 8-byte null descriptor required by Intel hardware architectures
    dd 0x0      ; 4 bytes of zeros
    dd 0x0      ; 4 bytes of zeros

GDT_CODE_SEG:   ; Segment descriptor for executable code
    dw 0xffff   ; Limit (bits 0-15): Defines the size of the segment (part 1)
    dw 0x0      ; Base  (bits 0-15): Defines the start memory address (part 1)
    db 0x0      ; Base  (bits 16-23): Defines the start memory address (part 2)
    db 10011010b ; Access Byte: 
                 ; - 1: Present (in memory)
                 ; - 00: Privilege level (Ring 0 - Kernel/Highest)
                 ; - 1: Descriptor type (Code/Data)
                 ; - 1: Executable (This is a Code segment)
                 ; - 0: Conforming (Accessible by lower privilege rings? No)
                 ; - 1: Readable (Allows reading constants from code)
                 ; - 0: Accessed bit (Managed by CPU)
    db 11001111b ; Flags & Limit:
                 ; - 1: Granularity (1 = Scale limit by 4KB, turning 0xFFFF into 4GB)
                 ; - 1: Size flag (1 = 32-bit protected mode segment)
                 ; - 0: Long mode flag (0 = Not 64-bit)
                 ; - 0: Available for OS use
                 ; - 1111: Limit (bits 16-19): The final bits of the segment size
    db 0x0      ; Base  (bits 24-31): Defines the start memory address (part 3)

GDT_DATA_SEG:   ; Segment descriptor for system data
    dw 0xffff   ; Limit (bits 0-15): Defines the size of the segment (part 1)
    dw 0x0      ; Base  (bits 0-15): Defines the start memory address (part 1)
    db 0x0      ; Base  (bits 16-23): Defines the start memory address (part 2)
    db 10010010b ; Access Byte:
                 ; - 1: Present
                 ; - 00: Privilege level (Ring 0)
                 ; - 1: Descriptor type
                 ; - 0: Executable (This is a Data segment)
                 ; - 0: Expansion direction (Grows up)
                 ; - 1: Writable (Allows writing data to RAM)
                 ; - 0: Accessed bit
    db 11001111b ; Flags & Limit: Identical to code segment (4GB flat sizing rules)
    db 0x0      ; Base  (bits 24-31): Defines the start memory address (part 3)

GDT_END:        ; Label marking the end of the GDT structure

GDT_DESC:       ; The GDT Descriptor structure passed to the 'lgdt' instruction
    dw GDT_END - GDT_BEGIN - 1 ; Calculate size of the GDT (True size minus 1)
    dd GDT_BEGIN               ; Physical memory address pointing to the start of the table

; Calculate segment selector offsets (Code is entry 1 at 0x08, Data is entry 2 at 0x10)
CODE_SEG equ GDT_CODE_SEG - GDT_BEGIN
DATA_SEG equ GDT_DATA_SEG - GDT_BEGIN

; --- BOOT SECTOR PADDING & SIGNATURE ---
times 510-($-$$) db 0 ; Pad remaining space with zeros until the file reaches 510 bytes
dw 0xaa55             ; Write the mandatory 2-byte BIOS boot magic signature (Total: 512 bytes)
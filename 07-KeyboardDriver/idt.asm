extern idt              ; Import the IDT entry array declared in kernel-base.c
extern isr1_Handler     ; Import the C language handling function for processing inputs
global isr1             ; Export assembly label address for IDT function vector binding
global loadIdt          ; Export entry-point execution label for the C kernel initialization

idtDesc:
    dw 2048             ; Size limit of the entire IDT structure allocation (256 entries * 8 bytes each - 1)
    dd idt              ; Direct 32-bit absolute physical memory address pointing to the IDT structure

; Low-level CPU execution context wrapper for Keyboard hardware line signals (IRQ 1)
isr1:
    pushad              ; Save all current general-purpose operational CPU registers onto the stack
    call isr1_Handler   ; Safely break out of assembly context to run the high-level C logic
    popad               ; Re-acquire all original general-purpose register parameters from the stack
    iretd               ; Perform an Interrupt Return, restoring EFLAGS, CS, and EIP register flags

; Flushes the IDT register architecture and turns on central processor interrupt parsing
loadIdt:
    lidt[idtDesc]       ; Force the physical x86 processor control unit to track our IDT allocation
    sti                 ; Set the Interrupt Flag (enabling hardware interrupts to reach the CPU)
    ret                 ; Pop return address pointer and yield control back to the caller
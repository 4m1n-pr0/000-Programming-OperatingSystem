# 07-KeyboardDriver

## Theory

The objective of this part is to build a keyboard driver that configures the Programmable Interrupt Controller (PIC), builds an Interrupt Descriptor Table (IDT), and captures raw scancodes from a PS/2 keyboard controller interface.

When a user type on a keyboard in a raw hardware environment, the signal follows a direct route through the computer's motherboard architecture before showing up on the screen.

#### How Keyboards Talk to Hardware

Modern USB keyboards natively simulate legacy standard PS/2 hardware behavior at the motherboard level. When a key is modified (pressed or released) :
- An internal microchip inside the keyboard layout sends a **Scan Code** representing the physical key's coordinates.
- The hardware signals **Interrupt Request Line 1 (IRQ 1)** on the Programmable Interrupt Controller.

#### The Role of the PIC (Programmable Interrupt Controller)

The PIC acts as a central switchboard matching hardware signals (IRQs) to system interrupt channels inside the CPU. This hardware component sits directly within modern chipsets. It arbitrates priorities so that critical hardware events take precedence. 
- **Master PIC (Port 0x20/0x21)**: Manages core system devices (Timer, Keyboard).
- **Slave PIC (Port 0xA0/0xA1)**: Cascades directly into the Master PIC to support secondary peripherals.

Because the system defaults map IRQ lines directly over critical standard CPU Exceptions (such as Division-by-Zero errors), `picRemap()` is implemented to move these offsets into an independent, collision-free block. In this implementation, the Master PIC is assigned to vector `0`, routing the keyboard handler directly through hardware interrupt index **`1`**.

#### The Interrupt Descriptor Table (IDT)
The IDT tells the CPU exactly what memory address to jump to when an interrupt fires. 


```bash
+-------------------------------------------------------+
|                IDT Array [0 ... 255]                  |
+-------------------------------------------------------+
| Vector 0 | Vector 1 (Keyboard) | ... |   Vector 255   |
+----------+---------------------+----------------------+
|
v
Points to: `isr1` (idt.asm assembly stub)
|
v
Calls:     `isr1_Handler` (kernel-base.c processing logic)
```

Each gate descriptor records:
- The 32-bit execution base offset of the processing code.
- The target Kernel Code Segment GDT Selector (`0x08`).
- System protection privilege attribute flags (`0x8E` marks ring-0 protection matching kernel space).

#### Reading the Scancode (Port 0x60)

When an interrupt event freezes normal CPU operational cycles, `isr1` safely saves processing state snapshots (`pushad`) and invokes `isr1_Handler()`. 

To extract what key triggered this event, the kernel reads the system bus register using assembly I/O instructions:
```c
inportb(0x60);
```

* **Crucial Behavior**: This port value must be read on every interrupt occurrence. Failing to empty the PS/2 controller data channel blocks the system from generating future input line alerts, effectively freezing keyboard functionality.

#### Layout Mapping Matrix

The raw hardware returns arbitrary index bytes rather than ASCII codes. A translation mapping matrix (`Scancode[]`) matches physical array coordinates directly into standard characters. When standard validation boundaries are reached, the translated key code flows directly into the raw VGA screen frame buffer addresses (`0xB8000`).

#### The EOI (End of Interrupt) Reset

Before returning control to the kernel, the software must signal the PIC that the current interrupt cycle has finished:

```c
outportb(0x20, 0x20); // Resets Master PIC
```

Without sending an EOI command, the hardware controller assumes the CPU is still stuck handling the current event, blocking all subsequent hardware signals.

## Practice

To build a specific operating system, execute this command : 
```bash
make VARIANT=?
```

To boot on the specific operating system, execute this command :
```bash
make run VARIANT=?
```

To clean the `bin` directory, execute this command :
```bash
make clean
```

> Note: The character `?` specifies the kernel variant to chose. If there was not this concept of variants, a different Makefile would be used for each specific kernel C program, which would be inefficient.
/* --- KEYBOARD & INT CONTROLLER REGISTER DEFINITIONS --- */

// Programmable Interrupt Controller (PIC) Ports
#define PIC1_C 0x20  // Master PIC Command register port. Used to send initialization commands & EOI signals.
#define PIC1_D 0x21  // Master PIC Data register port. Used for setting interrupt vectors, cascades, and masks.
#define PIC2_C 0xa0  // Slave PIC Command register port. Relays secondary hardware interrupts to the Master PIC.
#define PIC2_D 0xa1  // Slave PIC Data register port. Handles masks and vectors for IRQs 8-15.

// Initialization Command Words (ICW) Configuration Bitmasks
#define ICW1_DEF  0x10  // Initialization bit (bit 4). Tells the PIC that an initialization sequence is starting.
#define ICW1_ICW4 0x01  // ICW4 Needed bit (bit 0). Signals the PIC to expect a 4th initialization command word.
#define ICW4_x86  0x01  // 8086/88 Mode bit (bit 0). Sets the PIC to operate in an x86 architecture environment.

void cls();
void setMonitorColor(char);

void printString(char*);
void printChar(char);

void scroll();

void printColorString(char* , char);
void printColorChar(char , char);

void initIDT();
extern  void loadIdt();
extern  void isr1_Handler();
void handleKeypress(int);
void pressed(char);
void picRemap();

unsigned char inportb(unsigned short);
void outportb(unsigned short , unsigned char);

char* TM_START;
int CELL;

/**
 * Struct representing a single x86 Interrupt Descriptor Table (IDT) entry.
 * Tells the CPU where the Interrupt Service Routine (ISR) is located in memory
 * and what privilege levels are required to execute it.
 */
struct IDT_ENTRY{
    unsigned short base_Lower;  // The lower 16 bits of the ISR function pointer address
    unsigned short selector;    // The Code Segment selector in the GDT (usually 0x08)
    unsigned char zero;         // Unused/reserved field, must always be set to 0
    unsigned char flags;        // Gate type and attribute flags (e.g., privilege levels)
    unsigned short base_Higher; // The higher 16 bits of the ISR function pointer address
};

struct IDT_ENTRY idt[256];      // The IDT array supporting up to 256 interrupt vectors
extern unsigned int isr1;       // Pointer to the assembly ISR wrapper defined in idt.asm
unsigned int base;

int start(){
    TM_START = (char*) 0xb8000;
    CELL = 0;
    base = (unsigned int)&isr1; // Address of the ISR1

    cls();
    setMonitorColor(0x0a);

    char WELCOME[] = "Welcome to the 4M1N-OS :) !!!\n";
    char CLI_VERSION[] = "Command Line Version 1.0.0\n\n";
    char PROMPT[] = "4M1N-OS > ";

    printString(WELCOME);
    printString(CLI_VERSION);
    printColorString(PROMPT , 0x0a);

    initIDT(); // Call to the initIDT function
}


void cls(){
    int i = 0;
    CELL = 0;

    while(i < (2 * 80 * 25)){
        *(TM_START + i) = ' ';
        i += 2;
    }
}


void setMonitorColor(char Color){
    int i = 1;
    while(i < (2 * 80 * 25)){
        *(TM_START + i) = Color;
        i += 2;
    }
}


void printString(char* cA){
    int i = 0;
    while(*(cA + i) != '\0'){
        printChar(*(cA + i));
        i++;
    }
}

void printChar(char c){
    if(CELL == 2 * 80 * 25)
        scroll();
    if(c == '\n'){
        CELL = ((CELL + 160) - (CELL % 160));
        return;
    }
    *(TM_START + CELL) = c;
    CELL += 2;    
}

void scroll(){
    int i = 160 , y = 0;
    while(i < 2 * 80 * 25){
        *(TM_START + y) = *(TM_START + i);
        i += 2;
        y += 2;
    }
    CELL = 2 * 80 * 24;
    i = 0;
    while(i < 160){
        *(TM_START + CELL + i) = ' ';
        i += 2;
    }
}

void printColorString(char* c , char co){
    int i = 0;
    while(*(c + i) != '\0'){
        printColorChar(*(c + i) , co);
        i++;
    }
}

void printColorChar(char c , char co){
    if(CELL == 2 * 80 * 25)
        scroll();
    if(c == '\n'){
        CELL = ((CELL + 160) - (CELL % 160));
        return;
    }
    *(TM_START + CELL) = c;
    *(TM_START + CELL + 1) = co;
    CELL += 2;    
}

/**
 * Initializes the IDT entries, remaps the master/slave PIC layout,
 * unmutes the keyboard hardware line, and loads the IDT pointer into the CPU.
 */
void initIDT(){
    // Set up the interrupt gate for vector 1 (linked directly to IRQ 1 via picRemap)
    idt[1].base_Lower = (base & 0xFFFF);         // Extract the lower 16 bits of the assembly wrapper address
    idt[1].base_Higher = (base >> 16) & 0xFFFF;  // Extract the higher 16 bits of the assembly wrapper address
    idt[1].selector = 0x08;                      // Target the kernel code segment inside the GDT
    idt[1].zero = 0;                             // System field requirement set to zero
    idt[1].flags = 0x8e;                         // Present bit set, ring 0 privilege, 32-bit Interrupt Gate

    // Configuration phase: relocate PIC interrupts away from conflicting hardware registers
    picRemap();

    // Modify interrupt masks: 0xfd (11111101) enables IRQ 1 (Keyboard) on the Master PIC
    outportb(0x21 , 0xfd);
    // 0xff (11111111) mutes all interrupt requests routed to the Slave PIC
    outportb(0xa1 , 0xff);

    // Call assembly procedure to formally commit the IDT address configuration to CPU registers
    loadIdt();
}

/**
 * Communicates directly with physical hardware by reading an 8-bit byte from an I/O port.
 * @param _port The 16-bit physical hardware I/O address register
 * @return The 8-bit value returned by the device mapped to that port address
 */
unsigned char inportb(unsigned short _port){
    unsigned char rv;
    /* * Inline Assembly: "inb %1, %0"
     * Reads a byte from an I/O port register into the AL register.
     * Constraints Breakdown:
     * "=a" (rv)  : Output constraint. Tells the compiler to store the value of the 'AL' register into 'rv'.
     * "dN" (_port): Input constraint. Forces the compiler to load '_port' into the 'DX' register 
     * (or use an immediate constant if the port value fits in an 8-bit unsigned integer).
     */
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

/**
 * Communicates directly with physical hardware by writing an 8-bit byte to an I/O port.
 * @param _port The 16-bit physical hardware I/O address register
 * @param _data The 8-bit payload/command byte sent directly to the device
 */
void outportb(unsigned short _port, unsigned char _data){
    /* * Inline Assembly: "outb %1, %0"
     * Outputs a byte from the AL register to a specified hardware I/O port.
     * Constraints Breakdown:
     * "dN" (_port): Input operand %0. Loads port into 'DX' register (or treats as immediate literal if < 256).
     * "a" (_data) : Input operand %1. Loads the data payload byte directly into the 'AL' accumulator register.
     */
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

/**
 * High-level handler dispatched whenever a hardware keyboard interrupt occurs.
 * Reads raw keyboard activity data and dispatches an End-Of-Interrupt signal.
 */
extern void isr1_Handler(){
    // Read the scan code from the PS/2 keyboard controller data register (Port 0x60)
    handleKeypress(inportb(0x60));

    // Send the generic End-Of-Interrupt (EOI) signal (0x20) to Master and Slave PICs
    outportb(0x20 , 0x20);
    outportb(0xa0 , 0x20);
}

/**
 * Decodes a raw keyboard scan code into its readable ASCII counter-part.
 * @param code The 8-bit input scan code fetched from the physical input port
 */
void handleKeypress(int code){
    // Scan code mapping matrix, currently a mix between AZERTY and QWERTY :)
    char Scancode[] = {
        0 , 0 , '1' , '2' ,
        '3' , '4' , '5' , '6' ,
        '7' , '8' , '9' , '0' ,
        '-' , '=' , 0 , 0 , 'A' ,
        'Z' , 'E' , 'R' , 'T' , 'Y' ,
        'U' , 'I' , 'O' , 'P' , '[' , ']' ,
        0 , 0 , 'Q' , 'S' , 'D' , 'F' , 'G' ,
        'H' , 'J' , 'K' , 'L' , 'M' , '\'' , '`' ,
        0 , '\\' , 'W' , 'X' , 'C' , 'V' , 'B' , 'N' , ';' ,
        ',' , '.' , '/' , 0 , '*' , 0 , ' '
    };
    
    if(code == 0x1c)         // 0x1c matches the 'Enter/Return' key make code
        printChar('\n');
    else if(code < 0x3a)     // Filter out key release (break) codes and high special-key matrices
        pressed(Scancode[code]);
}

/**
 * Forwards a successfully decoded key character directly onto the visual screen cursor.
 * @param key The decoded ASCII character representation
 */
void pressed(char key){
    printChar(key);
}

/**
 * Remaps the Master and Slave PIC base vectors to clear conflicts.
 * Changes the base vector offset addresses from their defaults.
 */
void picRemap(){
    unsigned char a , b;
    // Cache the original hardware interrupt mask bytes to restore them afterwards
    a = inportb(PIC1_D);
    b = inportb(PIC2_D);

    // Broadcast Initialization Command Word 1 (ICW1) to both cascading PIC controller chips
    outportb(PIC1_C , ICW1_DEF | ICW1_ICW4);
    outportb(PIC2_C , ICW1_DEF | ICW1_ICW4);

    /* * ICW2: Assign Vector Offset bounds. 
     * Master starts at Vector 0x20 (32), Slave starts at Vector 0x28 (40).
     * * Note on your code: Your implementation passes 0 and 8 here. On x86, 
     * vectors 0-31 are explicitly reserved for CPU exceptions (like Page Faults / Div by Zero). 
     * Mapping them to 0 and 8 means a timer interrupt or a keyboard press looks identical to a 
     * CPU Exception crash! Usually, you would map these to 0x20 and 0x28.
     */
    outportb(PIC1_D , 0);
    outportb(PIC2_D , 8);

    // ICW3: Establish Master-Slave configuration wires.
    // 4 (bit 2 set: 00000100) tells the Master PIC that a slave is attached to its IRQ line 2.
    // 2 (literal value) tells the Slave PIC its identity is cascaded onto the Master's IRQ line 2.
    outportb(PIC1_D , 4);
    outportb(PIC2_D , 2);

    // ICW4: Configure environmental operating modes (instructs controllers to run in x86 mode)
    outportb(PIC1_D , ICW4_x86);
    outportb(PIC2_D , ICW4_x86);

    // Reapply saved hardware mask configurations back to operational status registers
    outportb(PIC1_D , a);
    outportb(PIC2_D , b);
}
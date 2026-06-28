void setMonitorColor(char);
void cls();
void printString(char*);

/**
 * High-level driver functions to interact with the ATA hard disk.
 */
void vid();
void put();
void get();

/**
 * External Assembly routines that handle the low-level I/O port manipulation 
 * required to communicate with the ATA IDE controller.
 */
extern void read();
extern void write();

/**
 * Global variables used to pass parameters between the C kernel and the 
 * Assembly disk driver routines.
 */
int blockAddr;   // Stores the target Logical Block Address (LBA) sector number.
char At[1024];   // Buffer holding the data to be written or read from the disk.

char* TM_START;


void blink(){
    setMonitorColor(0xa0);
    int TIME_OUT = 0x10fffff;
    while(--TIME_OUT);
    setMonitorColor(0x0a);
}


char strcmp(char* sou , char* dest){
    int i = 0;
    while(*(sou + i) == *(dest + i)){
        if(*(sou + i) == 0 && *(dest + i) == 0)
            return 1;
        i++;
    }
    return 0;
}

/**
 * Evaluates command-line inputs and triggers matching kernel or driver routines.
 * @param CMD Pointer to the string containing the user command.
 */
void strEval(char* CMD){
    char cmd1[] = "CLS";
    char cmd2[] = "COLORA";
    char cmd3[] = "COLORB";
    char cmd4[] = "COLORC";
    char cmd5[] = "COLORDEF";
    char cmd6[] = "VID";
    char cmd7[] = "HI";
    char cmd8[] = "PUT";
    char cmd9[] = "GET";

    char msg1[] = "\nHELLO , HAVE A GOOD JOURNEY LEARNING\n";

    if(strcmp(CMD , cmd1))
        cls();
    else if(strcmp(CMD , cmd2))
        setMonitorColor(0x3c);
    else if(strcmp(CMD , cmd3))
        setMonitorColor(0x5a);
    else if(strcmp(CMD , cmd4))
        setMonitorColor(0x2a);
    else if(strcmp(CMD , cmd5))
        setMonitorColor(0xa5);    
    else if(strcmp(CMD , cmd6))
        vid();
    else if(strcmp(CMD , cmd7))
        printString(msg1);
    
    /**
     * PUT Command Execution Block
     * Prepares data buffer with dummy payload and triggers disk persistence.
     */
    else if(strcmp(CMD , cmd8)){
        blockAddr = 0; // Target LBA sector 0 (Warning: Boot sector area)
        int i = 0;
        
        while(i < 511){
            At[i] = 'J'; // Fill the sector buffer payload with visual token 'J'
            i++;
        }
        At[i] = 0; // Null-terminate data string

        put(); // Invoke the hard disk writer execution path
        
        i = 0;
        while(i < 511){
            At[i] = 0;  // Clear the buffer in memory to prove subsequent read accuracy
            i++;
        }
    }        
    /**
     * GET Command Execution Block
     * Reads information from disk persistence back into RAM and displays it.
     */
    else if(strcmp(CMD , cmd9)){
        blockAddr = 0; // Target identical LBA sector 0
        get();         // Fetch sector data from disk via raw controller pipeline
        printString(At); // Dump retrieved buffer array strings to monitor
    }
        
}

/**
 * Fills video memory rapidly in an infinite loop to test performance thresholds.
 */
void vid(){
    char clr = 'A';
    while(1){
        int i = 0;
        while(i < 2 * 80 * 25){
            *(TM_START + i) = clr;
            clr++;
            i++;
        }
    }
}

/**
 * Interface layer wrapping the external raw x86 assembly disk write functionality.
 */
void put(){
    write();
}

/**
 * Interface layer wrapping the external raw x86 assembly disk read functionality.
 */
void get(){
    read();
}
void setMonitorColor(char);
void cls();
void printString(char*);
void vid();
void printHelp();
void printChar(char);

char* TM_START;

/**
 * Predefined 2D character array containing all supported system command strings.
 */
char CMDS[][21] = {"CLS", "COLORA", "COLORB", "COLORC", "COLORDEF", "VID", "HI", "HELP", "CUSTOM"};

/**
 * Flashes the monitor color momentarily to signal a boundary condition or an alert.
 */
void blink(){
    /* Temporary color assignment to initiate the alert state */
    setMonitorColor(0xa0);
    
    /* Hardcoded loop counter serving as a rudimentary timing delay */
    int TIME_OUT = 0x10fffff;
    while(--TIME_OUT);
    
    /* Re-application of the default monitor color profile post-delay */
    setMonitorColor(0x0a);
}

/**
 * Compares two null-terminated strings for exact equality character by character.
 * @param src Pointer to the source character string
 * @param dst  Pointer to the dst ination character string
 * @return 1 if strings match exactly, 0 otherwise
 */
char strcmp(char* src , char* dst ){
    int i = 0;
    
    /* Loop until a mismatch occurs or the terminating null characters are reached */
    while(*(src + i) == *(dst  + i)){
        /* If both characters are null terminators simultaneously, an exact match is confirmed */
        if(*(src + i) == 0 && *(dst  + i) == 0)
            return 1;
        i++;
    }
    return 0;
}

/**
 * Evaluates the input string against the command array and routes execution accordingly.
 * @param CMD Pointer to the character string containing the command input
 */
void strEval(char* CMD){

    /* Evaluates if the input matches the CLS command index */
    if(strcmp(CMD , CMDS[0]))
        cls();

    /* Evaluates if the input matches the COLORA command index */
    else if(strcmp(CMD , CMDS[1]))
        setMonitorColor(0x3c);

    /* Evaluates if the input matches the COLORB command index */
    else if(strcmp(CMD , CMDS[2]))
        setMonitorColor(0x5a);

    /* Evaluates if the input matches the COLORC command index */
    else if(strcmp(CMD , CMDS[3]))
        setMonitorColor(0x2a);

    /* Evaluates if the input matches the COLORDEF command index */
    else if(strcmp(CMD , CMDS[4]))
        setMonitorColor(0x0a);    

    /* Evaluates if the input matches the VID command index */
    else if(strcmp(CMD , CMDS[5]))
        vid();

    /* Evaluates if the input matches the HI command index */
    else if(strcmp(CMD , CMDS[6]))
        printString("\nHELLO , HAVE A GOOD JOURNEY LEARNING\n");

    /* Evaluates if the input matches the HELP command index */
    else if(strcmp(CMD , CMDS[7]))
        printHelp();

    /* Evaluates if the input matches the CUSTOM command index */
    else if(strcmp(CMD , CMDS[8]))
        printString("\nHERE IS A CUSTOM MESSGAGE\n");
}

/**
 * Fills video memory continuously with raw ascending ASCII values.
 * Infinitely cycles visual patterns across the display terminal.
 */
void vid(){

    char clr = 'A';

    /* Infinite loop trapping execution to generate a persistent video output pattern */
    while(1){
        int i = 0;
        /* Iterates through the entire text-mode memory space byte-by-byte */
        while(i < 2 * 80 * 25){
            *(TM_START + i) = clr;
            clr++;
            i++;
        }
    }

}

/**
 * Prints the help interface layout along with a list of valid terminal commands.
 */
void printHelp(){

    printString("\n\nTHIS IS THE HELP MESSAGE :)\n");
    printString("HERE IS A LIST OF COMMANDS YOU CAN TYPE !\n\n");

    int i = 0;
    /* Loops through the total fixed count of 9 supported command keywords */
    for(i=0; i<9; i++){
        printString(CMDS[i]);
        printChar('\n');
    }

}
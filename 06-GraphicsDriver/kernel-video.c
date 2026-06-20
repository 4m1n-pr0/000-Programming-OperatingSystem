void cls();
void setMonitorColor(char);

void printString(char*);
void printChar(char);

void scroll();

void printColorString(char* , char);
void printColorChar(char , char);

// Global variables to track the VGA text buffer pointer and current cursor position
char* TM_START; // Pointer to the start of the video memory (0xb8000)
int CELL;       // Current byte offset in the video memory buffer

int start()
{
    char* TM_START = (char*) 0xb8000;
    int i;
    char obj = 0;
    while(1)
    {
        i = 0;
            while(i < (2 * 80 * 25))
            {
                *(TM_START + i) = obj;
                i++;
                obj++;
            }
    }
 }

/**
 * Clears the screen by overwriting all character slots with space characters.
 * Keeps existing text/attribute colors intact.
 */
void cls(){
    int i = 0;
    CELL = 0; // Reset the write cursor to the beginning
    // Loop through the 80 columns * 25 rows grid. Each cell is 2 bytes (character + attribute)
    while(i < (2 * 80 * 25)){
        *(TM_START + i) = ' '; // Write a space character to the character byte
        i += 2;                // Skip the attribute (color) byte
    }
}

/**
 * Changes the color attributes of the entire screen without changing the text.
 * @param Color The 8-bit color attribute byte (Background << 4 | Foreground)
 */
void setMonitorColor(char Color){
    int i = 1; // Start at index 1 to target the attribute bytes
    while(i < (2 * 80 * 25)){
        *(TM_START + i) = Color; // Apply the color attribute
        i += 2;                  // Skip the character byte
    }
}

/**
 * Prints a null-terminated string using the default/existing screen colors.
 * @param cA Pointer to the null-terminated character array
 */
void printString(char* cA){
    int i = 0;
    while(*(cA + i) != '\0'){
        printChar(*(cA + i)); // Print each character sequentially
        i++;
    }
}

/**
 * Prints a single character at the current cursor position.
 * Handles newlines and triggers vertical scrolling if the screen bounds are exceeded.
 * @param c The character to print
 */
void printChar(char c){
    // If the cursor reaches the end of the 80x25 buffer, trigger a scroll up
    if(CELL == 2 * 80 * 25)
        scroll();
        
    // Handle the newline character
    if(c == '\n'){
        // Move the cursor to the beginning of the next line (aligns to multiples of 160 bytes)
        CELL = ((CELL + 160) - (CELL % 160));
        return;
    }
    
    *(TM_START + CELL) = c; // Write character to the current video memory slot
    CELL += 2;              // Advance cursor by 2 bytes (skips the color byte)
}

/**
 * Scrolls the entire screen up by one row.
 * Copies rows 1-24 into rows 0-23, and clears the newly created bottom row (row 24).
 */
void scroll(){
    int i = 160 , y = 0; // i starts at the beginning of the second row (80 cells * 2 bytes = 160)
    
    // Shift all character data up by 160 bytes (one full row)
    while(i < 2 * 80 * 25){
        *(TM_START + y) = *(TM_START + i);
        // Note: This only shifts character bytes. Attribute bytes are skipped due to += 2
        i += 2; 
        y += 2;
    }
    
    // Reposition the cursor to the beginning of the last row
    CELL = 2 * 80 * 24;
    
    // Clear the newly empty bottom row with spaces
    i = 0;
    while(i < 160){
        *(TM_START + CELL + i) = ' ';
        i += 2;
    }
}

/**
 * Prints a null-terminated string with a specified custom color.
 * @param c Pointer to the null-terminated character array
 * @param co The 8-bit color attribute byte to apply to the string
 */
void printColorString(char* c , char co){
    int i = 0;
    while(*(c + i) != '\0'){
        printColorChar(*(c + i) , co); // Print each character with the custom color
        i++;
    }
}

/**
 * Prints a single character with a specified custom color attribute.
 * @param c The character to print
 * @param co The 8-bit color attribute byte
 */
void printColorChar(char c , char co){
    // Trigger scroll if the screen memory boundary is reached
    if(CELL == 2 * 80 * 25)
        scroll();
        
    // Handle the newline character
    if(c == '\n'){
        CELL = ((CELL + 160) - (CELL % 160));
        return;
    }
    
    *(TM_START + CELL) = c;       // Write character byte
    *(TM_START + CELL + 1) = co;  // Write custom color attribute byte
    CELL += 2;                    // Advance cursor past both bytes
}
int start()
{
    char* cell = (char*) 0xb8000; // Base address VGA Text Mode
    int i = 0; // Starting at 0, because it is a line of colored characters that is about to be printed

    while(i < (2 * 80)) // Only the first line is changed
    {
        *(cell + i) = 'J'; // Fills the character with J
        *(cell + i + 1) = 0x15; // Assigne the color of the pixel (thus i+1)
        i += 2; // Move to the next byte
    }
}
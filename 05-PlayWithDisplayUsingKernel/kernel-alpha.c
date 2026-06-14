int start()
{
    char* cell = (char*) 0xb8000; // Base address VGA Text Mode
    int i = 1; // Starting at one, otherwise would modify characters and not colors
    char color = 0; // Start at 0 so the first color will be set at 0, the next at one, and so on

    while(i < 2 * 80 * 25)
    {
        *(cell + i) = color; // Assign the color value to the second byte of the pixel
        i += 2; // Incrementing the variable to move on to the color of the next pixel
        color++; // Increment the color value
    }
}
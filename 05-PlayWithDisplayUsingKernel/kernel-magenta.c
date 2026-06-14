int start()
{
	char* cell = (char*) 0xb8000; // Base address VGA Text Mode
	int i = 1; // Starting at one, otherwise would modify characters and not colors
	
	while(i < (2 * 80 * 25)) // The matrix of pixels is 80 * 25, and each pixel is in two bytes
	{
		*(cell + i) = 0x59; // Assign a value to the second byte of the pixel (color)
		i += 2; // Incrementing the variable to move on to the color of the next pixel
	}
}
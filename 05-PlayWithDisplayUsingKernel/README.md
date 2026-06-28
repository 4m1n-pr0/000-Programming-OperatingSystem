# 05-PlayWithDisplayUsingKernel

## Theory

In this part, there are different kernels with wich it is possible to play with display.

As a reminder, each character in Text Mode is written into two bytes. The first is for the character itself, while the second is for the background color (first four bits) and the foreground color (last four bits).

In the `kernel-magenta.c`, the background color is set to `magenta` and the foreground color to `light blue`. This is done by iterating over the matrix of pixels, and assigning a value at the second byte only.

Next, in the `kernel-chars.c`, a single line of character is printed with a defined color.

Finally, in the `kernel-alpha.c`, the background and foreground color of each pixel on screen is modified. Thus, the result appear like random rectangles with each a different color.

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

> Note: The character `?` specifies the variant to chose. In the current case, variants of kernel are 'alpha', 'chars' and 'magenta'. If there was not this concept of variants, a different Makefile would be used for each specific kernel C program, which would be inefficient.
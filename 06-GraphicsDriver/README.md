# 06-GraphicsDriver

## Theory

#### Driver

This part aims to develop a lightweight, bare-metal graphics driver (designed for x86 operating systems) operating in VGA Text Mode (80x25 configuration). This driver directly interfaces with the memory-mapped I/O hardware buffer to handle text rendering, terminal colors, line wrapping, and vertical scrolling without relying on underlying standard libraries or BIOS interrupts.

Its features are the following :
* **Direct Hardware Mapping:** Directly manipulates the standard VGA text buffer located at memory address `0x0B8000`.
* **Dynamic Hardware Scrolling:** Automatically detects when text reaches the screen boundary and shifts the terminal grid upward by one row while clearing the trailing line.
* **Color Customization:** Supports full 8-bit color attribute mapping (4-bit background, 4-bit foreground) globally or on a per-character basis.
* **Control Character Parsing:** Features built-in parsing for newline (`\n`) characters to handle programmatic alignment and text structures.

#### Play with driver by making a video

Videos are only sequences of pictures. When the delay between each image is small enough, there is a feeling of a video. Hence, there is a `kernel-video.c` that implements this idea. The program will change the video memory section in a rapid way.

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

> Note: The character `?` specifies the kernel variant to chose. In the current case, variants of kernel are 'base', and 'video'. If there was not this concept of variants, a different Makefile would be used for each specific kernel C program, which would be inefficient.
# 08-FirstPrototype

## Theory

This milestone presents the first functional prototype of the operating system. A major architectural addition is the integration of the `extra.h` add-on module. This header file serves as the core utility interface, introducing crucial abstractions for terminal interaction, string evaluation, and custom command parsing (`strcmp`, `strEval`, `printHelp`). This decouples higher-level shell behavior from the foundational kernel logic. Please look at the `extra.h` program for more information as comments.

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
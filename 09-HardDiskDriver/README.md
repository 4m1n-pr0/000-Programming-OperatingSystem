# 09-HardDiskDriver

## Theory

This step includes a hard disk driver for legacy Parallel Advanced Technology Attachment (PATA) technology using IDE (Integrated Drive Electronics) controller ports. The driver implements **Programmed Input/Output (PIO)** mode with **28-bit Logical Block Addressing (LBA)** to read and write sectors. 

#### Why is the Driver Written in Assembly Instead of C?

While high-level algorithms are easily managed in C, low-level architecture initialization requires direct control over specialized CPU instructions. Operating system architectures use Assembly for disk drivers due to the following structural requirements:
- **Direct Bus I/O Instructions:** C does not feature native syntax for raw hardware port mapping. Specialized Assembly instructions, such as `insw` (Input String Word) and `outsw` (Output String Word), are required to stream data arrays between system RAM and motherboard controller ports (e.g., `0x1F0`).
- **Precise Context Preservation:** Low-level I/O tasks modify underlying CPU registers directly. Assembly allows exact register management (`pushfd`/`popfd` for the CPU flags register) to ensure CPU stability when transferring control between hardware subsystems and the core kernel.

#### Storage Addressing (LBA Mode)

The driver communicates via **LBA (Logical Block Addressing)** mode. Rather than passing complex Cylinder-Head-Sector geometries, the disk is treated as a linear array of blocks numbered from 0 upwards. 

Each block (sector) contains **512 bytes**. Passing a block address of `0` targets the first sector on the drive—historically reserved for the Master Boot Record (MBR) / Bootloader payload.

> ⚠️ **CRITICAL WARNING:** Writing arbitrary strings directly to sector `0` overwrites valid boot sector partitions. While safely sandboxed inside a virtual emulator environment, doing this on bare physical hardware renders a storage drive unbootable until a functional bootloader is reinstalled.

#### Components

**`extra.h` (Buffer & Parameter Management)**
   * Defines a persistent memory buffer array (`At[1024]`) and a logical block pointer tracker (`blockAddr`).
   * Evaluates console execution pipelines via `PUT` (clears memory, floods the buffer with placeholder `'J'` constants, and flushes bytes to disk) and `GET` (calls low-level reads, updates the target RAM tracking index, and outputs data strings to the monitor).

**`ata.asm` (Hardware Command Pipeline)**
   * Directly programs the standard Primary ATA Bus ports (`0x1F2` - `0x1F7`).
   * Polls the IDE status registers until hardware asserts readiness via the Data Request flag (`DRQ`, bit 3).
   * Moves raw binary streams into x86 processing cores using execution block hardware speed optimizations (`rep insw` / `rep outsw`).


## Practice

To build a specific operating system, execute this command: 
```bash
make VARIANT=?
```

To boot on the specific operating system, execute this command:

```bash
make run VARIANT=?
```

To clean the `bin` directory, execute this command:

```bash
make clean
```

> **Note:** The character `?` specifies the kernel variant to choose. If there was not this concept of variants, a different Makefile would be used for each specific kernel C program, which would be inefficient.
[bits 32]

; External shared references linked between the C kernel and this Driver
extern blockAddr
extern At
global read
global write

; Reads a single sector from the hard disk using 28-bit Logical Block Addressing (LBA).
; Employs Programmed I/O (PIO) mode by directly probing x86 bus ports.

read:
        ; Preserve EFLAGS register state and target registers to prevent context corruption
        pushfd
        and eax , 0x0FFFFFFF
        push eax
        push ebx
        push ecx
        push edx
        push edi

        ; Retrieve parameters from shared C global symbols
        mov eax , [blockAddr] ; Load 32-bit Logical Block Address target variable
        mov cl , 1           ; Hardcode sector count payload modifier to 1 sector (512 bytes)
        mov edi , At         ; Destination address pointer where incoming disk data is written

        mov ebx , eax        ; Duplicate base LBA configuration into EBX for bitwise division
 
        ; Drive/Head Register Port Configuration (0x01F6)
        mov edx , 0x01F6 
        shr eax , 24         ; Isolate bits 24-27 of the target LBA
        or al , 11100000b     ; Force bits 5, 7 for historical layout, bit 6 to toggle LBA mode
        out dx , al          ; Dispatch selection payload byte to drive selection port
 
        ; Sector Count Register Port Configuration (0x01F2)
        mov edx , 0x01F2      
        mov al , cl           ; Load sector counts parameter string from CL
        out dx , al
 
        ; LBA Low Register Port Configuration (0x1F3)
        mov edx , 0x1F3       
        mov eax , ebx         ; Reload native LBA address value
        out dx , al           ; Send bits 0-7 of the LBA address block
 
        ; LBA Mid Register Port Configuration (0x1F4)
        mov edx , 0x1F4       
        mov eax , ebx         
        shr eax , 8           ; Shift right to isolate bits 8-15
        out dx , al           ; Send bits 8-15 of the LBA address block
 
        ; LBA High Register Port Configuration (0x1F5)
        mov edx , 0x1F5       
        mov eax , ebx         
        shr eax , 16          ; Shift right to isolate bits 16-23
        out dx , al           ; Send bits 16-23 of the LBA address block
 
        ; Command Register Port Configuration (0x1F7)
        mov edx , 0x1F7       
        mov al , 0x20         ; Opcode 0x20: Read Sectors With Retry command
        out dx , al
 
.loop1:
        ; Status polling cycle: Wait for drive controller assertion signals
        in al , dx            ; Read Status register contents from port 0x1F7
        test al , 8           ; Mask out Data Request (DRQ) flag bit (bit 3)
        jz .loop1             ; Loop until controller declares memory sector buffer ready
 
        ; Calculate cumulative word size allocation calculations
        mov eax , 256         ; Standard structural constant: 1 sector comprises 256 words (512 bytes)
        xor bx , bx
        mov bl , cl           ; Multiply base word block against the requested sector allocation
        mul bx
        mov ecx , eax         ; Transfer computed total word loops loop into iteration counter ECX
        mov edx , 0x1F0       ; Hard disk Data Port address for transferring physical signals
        rep insw              ; Fast Assembly string operational loop: reads words from DX into [EDI]
 
        ; Restore execution environments prior to context return sequences
        pop edi
        pop edx
        pop ecx
        pop ebx
        pop eax
        popfd
        ret


; Writes a single sector to the hard disk using 28-bit Logical Block Addressing (LBA).
; Employs Programmed I/O (PIO) mode by directly probing x86 bus ports.

write:
        ; Safely preserve critical system context and operational registers on the stack
        pushfd
        and eax , 0x0FFFFFFF
        push eax
        push ebx
        push ecx
        push edx
        push edi

        ; Retrieve operational parameters from shared C memory variables
        mov eax , [blockAddr] ; Load the target block sector allocation metric
        mov cl , 1           ; Configure write process boundaries strictly to 1 sector
        mov edi , At         ; Source memory address pointer referencing output byte data strings

        mov ebx , eax        ; Duplicate internal variable metrics for staging

        ; Configure hard disk communication channel targeting structural bus parameters
        mov edx , 0x01F6
        shr eax , 24         ; Isolate highest bits 24-27 of LBA variable
        or al , 11100000b     ; Force LBA targeting parameters active via bitmasks
        out dx , al
 
        ; Assign internal transfer size targets to device controller port
        mov edx , 0x01F2
        mov al , cl           
        out dx , al
 
        ; Dispatch LBA address bytes across designated IDE bus slots
        mov edx , 0x1F3
        mov eax , ebx         ; Sector tracking bits 0-7 loaded
        out dx , al
 
        mov edx , 0x1F4
        mov eax , ebx         
        shr eax , 8           ; Sector tracking bits 8-15 shifted
        out dx , al
 
        mov edx , 0x1F5
        mov eax , ebx         
        shr eax , 16          ; Sector tracking bits 16-23 shifted
        out dx , al
 
        ; Issue physical command token over hardware bus architecture
        mov edx , 0x1F7
        mov al , 0x30         ; Opcode 0x30: Write Sectors With Retry command
        out dx , al
 
.loop2:
        ; Status polling cycle: Validate structural device write buffers are empty and ready
        in al , dx
        test al , 8           ; Check Data Request (DRQ) flag state metrics
        jz .loop2             ; Halt processing sequence until state flag resolves active
 
        ; Process quantitative structure mapping conversions
        mov eax , 256         ; Core metric conversion count (256 words = 512 data bytes)
        xor bx , bx
        mov bl , cl           
        mul bx
        mov ecx , eax         ; Set total word transmission targets inside counter register ECX
        mov edx , 0x1F0       ; Direct routing map to storage controller data I/O port address
        mov esi , edi         ; Relocate buffer tracking memory pointer directly to ESI source index
        rep outsw             ; Stream sequence memory components sequentially out through I/O port address
 
        ; Restore pristine environment variables and break stack context structures back clean
        pop edi
        pop edx
        pop ecx
        pop ebx
        pop eax
        popfd
        ret
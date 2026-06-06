mov ah, 0x0e

infinite:
    mov al , 'H'
    int 0x10
    jmp infinite

times 510-($-$$) db 0

dw 0xaa55
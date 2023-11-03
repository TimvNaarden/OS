; Read some sectors from the boot disk using our disk_read function
[bits 16]
[org 0x7c00]
KERNEL_OFFSET equ 0x1000

    mov [BOOT_DRIVE], dl ; BIOS stores our boot drive in DL , so it ’s
                            ; best to remember this for later.
    mov bp, 0x8000 ; Here we set our stack safely out of the
    mov sp, bp ; way , at 0 x8000

    mov bx, MSG_REAL_MODE
    call print16
    call print16_nl

    mov bx, KERNEL_OFFSET
    mov dh, 50
    mov dl, [BOOT_DRIVE]

    call disk_load

    call switch_to_pm
    jmp $
%include "boot/print/print16.asm"
%include "boot/print/print32.asm"
%include "boot/disk_load.asm"
%include "boot/gdt.asm"
%include "boot/move_to_pm.asm"

[bits 32]
BEGIN_PM:
    mov ebx, MSG_PROT_MODE
    call print32
;   jmp $
    call KERNEL_OFFSET
    jmp $

; Global variables
BOOT_DRIVE: db 0
MSG_REAL_MODE: db "Started in 16 Bit Real Mode", 0
MSG_PROT_MODE: db "Succesfully landed in 32-bit protected mode", 0

; Bootsector padding
times 510 -($ - $$) db 0
dw 0xaa55

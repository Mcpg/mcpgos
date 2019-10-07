    [bits 32]
    [cpu 686]
    
; %1 - interrupt number
%macro InterruptHandler 1

    global _Int_%1
    _Int_%1:
        push dword 0x00000000
        push dword %1
        jmp IntCommon

%endmacro

; %1 - interrupt number
%macro ExceptionHandler 1

    global _Int_%1
    _Int_%1:
        push dword %1
        jmp IntCommon

%endmacro

section .text
    
    ; Exceptions
    InterruptHandler 0x00 ; #DE
    InterruptHandler 0x01 ; #DB
    InterruptHandler 0x02 ; NMI
    InterruptHandler 0x03 ; #BP
    InterruptHandler 0x04 ; #OF
    InterruptHandler 0x05 ; #BR
    InterruptHandler 0x06 ; #UD
    InterruptHandler 0x07 ; #NM
    ExceptionHandler 0x08 ; #DF
    InterruptHandler 0x0A ; #TS
    ExceptionHandler 0x0B ; #NP
    ExceptionHandler 0x0C ; #SS
    ExceptionHandler 0x0D ; #GP
    ExceptionHandler 0x0E ; #PF
    InterruptHandler 0x10 ; #MF
    ExceptionHandler 0x11 ; #AC
    InterruptHandler 0x12 ; #MC
    InterruptHandler 0x13 ; #XM / #XF
    InterruptHandler 0x14 ; #VE
    ExceptionHandler 0x1E ; #SX
    
    ; IRQ handlers
    InterruptHandler 0x20
    InterruptHandler 0x21
    InterruptHandler 0x22
    InterruptHandler 0x23
    InterruptHandler 0x24
    InterruptHandler 0x25
    InterruptHandler 0x26
    InterruptHandler 0x27
    InterruptHandler 0x28
    InterruptHandler 0x29
    InterruptHandler 0x2A
    InterruptHandler 0x2B
    InterruptHandler 0x2C
    InterruptHandler 0x2D
    InterruptHandler 0x2E
    InterruptHandler 0x2F
    
    ; Syscall interrupt
    InterruptHandler 0x83
    
    global _IntDefault:function (_IntDefault.end - _IntDefault)
    _IntDefault:
        iret
        .end:
    
    IntCommon:
        ; Save the processor state
        pushad
        push ds
        push es
        push fs
        push gs
        
        push esp
        extern HLIntHandler ; High-Level Interrupt Handler
        call HLIntHandler
        mov esp, eax
        
        ; Restore the processor state
        pop gs
        pop fs
        pop es
        pop ds
        popad
        add esp, 8
        iret
        .end:
    
    global IdtLoad:function (IdtLoad.end - IdtLoad)
    IdtLoad:
        lidt [IdtPointer]
        ret
        .end:

section .data
    
    IdtPointer:
        extern IdtTable
        dw 256 * 8
        dd IdtTable
        .end:


    [bits 32]
    [cpu 686]
    
;  %1 - base
;  %2 - limit
;  %3 - access
;  %4 - flags
%macro GDT_ENTRY 4
    dw (%2 & 0xFFFF)
    dw (%1  & 0xFFFF)
    db (%1 >> 16) & 0xFF
    db %3
    db (((%2 >> 16) & 0x0F) | %4 & 0xF0)
    db (%1 >> 24) & 0xFF
%endmacro

%define GDT_ENTRY_FULL_SPAN(access) \
    GDT_ENTRY 0, 0x000FFFFF, access, 0xC0
    
section .text

    global GdtInit:function (GdtInit.end - GdtInit)
    GdtInit:
        push ebp
        mov ebp, esp
        
        lgdt [GdtPointer]
        
        ; Load the data segments
        mov eax, 0x10
        mov ds, eax
        mov ss, eax
        mov es, eax
        mov fs, eax
        mov gs, eax
        
        ; Load the code segment
        jmp 0x08:.cont
        .cont:
        
        ; Write TSS address into the GDT
        mov ecx, TSS
        mov word [GdtTable.tss + 2], cx
        shr ecx, 16
        mov byte [GdtTable.tss + 4], cl
        mov byte [GdtTable.tss + 7], ch
        
        extern StackTop
        push dword StackTop
        call TssSetEsp
        add esp, 4

        ; Load the TSS
        mov eax, 0x28
        ltr ax
        
        pop ebp
        xor eax, eax
        ret
        .end:
    
    ;void TssSetEsp(uintptr_t newEsp)
    global TssSetEsp:function (TssSetEsp.end - TssSetEsp)
    TssSetEsp:
        mov eax, dword [esp + 4]
        mov dword [TSS.esp], eax
        xor eax, eax
        ret
        .end:

section .data
    
    global GdtTable:data (GdtTable.end - GdtTable)
    GdtTable:
        ; NULL entry
        dq 0
        
        ; Kernel code and data
        GDT_ENTRY_FULL_SPAN(0x9A)
        GDT_ENTRY_FULL_SPAN(0x92)
        
        ; User code and data
        GDT_ENTRY_FULL_SPAN(0xFA)
        GDT_ENTRY_FULL_SPAN(0xF2)
        
        ; TSS (base to be set by GdtInit)
        .tss:
        GDT_ENTRY 0, 104, 0x89, 0x40
        
        .end:
    
    global GdtPointer:data (GdtPointer.end - GdtPointer)
    GdtPointer:
        dw GdtTable.end - GdtTable
        dd GdtTable
        .end:
    
    TSS:
        times 1 dd 0
        .esp: dd 0x00000000
        .ss: dd 0x10
        times 91 dd 0
        .iomapBase: dd 104 << 16
        .end:
    
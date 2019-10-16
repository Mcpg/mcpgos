    [bits 32]
    [cpu 686]

%define BOOT_ERROR_MBOOT_TOO_BIG 0xDEAD0001

%define STACK_SIZE 65536
%define KERNEL_VIRTUAL 0xE0000000
%define KERNEL_PHYSICAL 0x100000

%define TO_PHYSICAL(virt) \
    (virt - KERNEL_VIRTUAL + KERNEL_PHYSICAL)

section .multiboot
    
    align 8
    global MultibootHeader:data (MultibootHeader.end - MultibootHeader)
    MultibootHeader:
    
    dd 0xE85250D6 ; Magic
    dd 0x00000000 ; i386
    dd (MultibootHeader.end - MultibootHeader) ; Header size
    dd -0xE85250D6 - (MultibootHeader.end - MultibootHeader) ; Checksum
    
    ; Module alignment tag
    align 8
    dw 0x0006
    dw 0x0000
    dd 0x00000008
    
    ; Tag termination tag
    align 8
    dw 0x0000
    dw 0x0000
    dd 0x00000008
        
    .end:
    
section .text

    global _start:function (_start.end - _start)
    _start:
        
        ;
        ; The bootloader dropped us off here.
        ; This part of the system initialization consists of:
        ;   * Initializing paging and map the kernel where it needs
        ;     to be mapped
        ;   * Calling C function kmain
        ;
        ; NOTE: _start does not check if the magic value in eax is correct.
        ;       It's the user's fault for using the wrong bootloader ;-)
        ;
        ; NOTE2: The ebx register MUST NOT be modified, since it contains
        ;        a pointer to the Multiboot2 info struct which isn't saved
        ;        anywhere
        ;
        
        cli
        
        ; Check the size of Multiboot tags (see include/McpgOS.hpp)
        cmp dword [ebx], 4096
        jle .copyMboot
        
        mov eax, BOOT_ERROR_MBOOT_TOO_BIG
        jmp .hang
        
        ; Copy the multiboot tags to the kernel space
        .copyMboot:
            mov ecx, dword [ebx]
            mov esi, ebx
            mov edi, TO_PHYSICAL(MultibootTags)
            rep movsb
        ; From now, ebx can be freely used
        
        ; Prepare paging
        ;;;;;;;;;;;;;;;;;
        
        ; Prepare kernel space pages
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        
        extern _KernelStart
        extern _KernelEnd
        
        mov eax, TO_PHYSICAL(_KernelStart)
        mov esi, TO_PHYSICAL(KernelPageTables)
        
        .kernelSpacePreparing:
            ; Insert the address
            or dword [esi], eax
            
            ; Mark the page as present, writable
            or dword [esi], 0b000000011
            
            add esi, 4
            add eax, 4096
            cmp eax, TO_PHYSICAL(_KernelEnd)
            jl .kernelSpacePreparing
        
        ; Prepare temoporary page directory
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        
        ; Mapping of the first 4 MiB of address space
        .identityPageMapping:
            or dword [TO_PHYSICAL(TemporaryPageDirectory)], \
                TO_PHYSICAL(_ZeroIdentityPage)
        
        ; Mapping of the kernel space
        .kernelSpaceMapping:
            mov ecx, 128
            mov esi, TO_PHYSICAL(TemporaryPageDirectory.kernelSpace)
            mov edi, TO_PHYSICAL(KernelPageTables)
            .kernelSpaceMapping.loop:
                
                or dword [esi], edi
                or dword [esi], 0b000000011
                
                add esi, 4
                add edi, 4096
                loop .kernelSpaceMapping.loop
        
        ; Enable paging
        extern CurrentPageDirectory
        mov dword [CurrentPageDirectory], TemporaryPageDirectory
        
        mov eax, TO_PHYSICAL(TemporaryPageDirectory)
        mov cr3, eax
        
        mov eax, cr0
        or eax, 0x80000000
        mov cr0, eax
        
        ; Here the CPU either page faults or continues to jump to KMain.
        
        mov esp, StackTop
        cld
        
        extern KMain
        mov eax, KMain
        jmp eax
        
        .hang:
            cli
            hlt
            jmp .hang
        
        .end:
        
section .data
    
    ; KernelPageTables map whole 512 MiB of kernel space
    ; (from 0xE0000000 to the end of address space)
    align 4096
    global KernelPageTables:data (KernelPageTables.end - KernelPageTables)
    KernelPageTables:
        %rep 128
            %rep 1024
                dd 0b00000000000000000000000000000000
            %endrep
        %endrep
        .end:
    
    ; Page table identity mapping first 4 MiB
    align 4096
    _ZeroIdentityPage:
        %assign i 0
        %rep 1024
            dd i | 0b00000011
            %assign i i + 4096
        %endrep
        
    
    align 4096
    TemporaryPageDirectory:
        ; Identity map first 4 MiB (to be filled by code)
        dd 0 | 0b000000011
        
        ; Mark these pages as non-present
        times (1024 - 128 - 1) dd 0
        
        ; Kernel space mapping (to be filled by code)
        .kernelSpace:
        times 128 dd 0
        
        
section .bss
    
    align 4096
    resb STACK_SIZE
    StackTop:
    
    global MultibootTags:data (MultibootTags.end - MultibootTags)
    MultibootTags:
        resb 4096
        .end:

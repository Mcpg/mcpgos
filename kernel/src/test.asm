    [bits 32]

section .text

    global TestFunction
    TestFunction:

        mov eax, 0x01
        mov ebx, message
        mov ecx, 18
        int 0x83

        jmp $

        message: db "Hello, userspace!", 10


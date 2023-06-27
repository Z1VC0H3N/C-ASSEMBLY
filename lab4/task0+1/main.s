SECTION .data
    argv db "the argv is %s" ,10,0
    argc db "the argc is %d",10,0
    ;comperation db "ecx is %d eax is %d",10,0 

SECTION .text
    extern printf
    global main
    main:
        push ebp
        mov ebp, esp
        ;pushad
        mov eax, DWORD [ebp+8]
        pushad
        push DWORD eax
        push argc
        call printf
        add esp, 8
        popad
        mov ebx, DWORD [ebp+12]
        mov ecx, 0
        start_loop:
            pushad
            ;push DWORD eax
            ;push DWORD ecx
            ;push comperation
            ;call printf
            ;add esp, 12
            push DWORD [ebx]
            push argv
            call printf
            add esp, 8
            popad
            inc ecx
            add ebx, 4
            cmp eax, ecx
            jne start_loop
        mov esp,ebp
        pop ebp
        ret


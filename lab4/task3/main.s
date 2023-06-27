SECTION .data
    argv db "1.%p 2.%d",10,0 
    argc db "the argc is %d",10,0
    size db "%02x",0
    c db "%08x",10,0

SECTION .text
    extern printf
    extern malloc
    extern free
    global main
    y_struct: dd 5
    y_num: db 0xaa,1,2,0x44,0x4f
    x_struct: dd 6
    x_num: db 0xaa, 1,2,3,0x44,0x4f
    main:
        push ebp
        mov ebp, esp
        pushad
        push x_struct
        push y_struct
        call add_multi
        add esp,8 
        popad
        mov esp,ebp
        pop ebp
        ret

    

    add_multi:
        push ebp
        mov ebp, esp
        mov eax, DWORD [ebp+8]
        mov ecx, DWORD [eax]
        mov ebx,DWORD[ebp+12]
        mov edx,DWORD [ebx]
        call Get_MaxMin
        mov ecx, DWORD [eax]
        mov edx,DWORD [ebx]
        add eax, 4
        add ebx, 4 ; from this point eax is the bigger.
        
        mov esi, DWORD ecx
        add esi, DWORD 4
        push ebx
        push ecx
        push edx 
        push eax
        push esi
        call malloc
        add esp, 4 
        mov esi,DWORD eax
        pop eax
        pop edx
        pop ecx
        pop ebx   ;malloc to the size of the big array +1. the address in esi
        mov [esi],ecx




        sub esp, 20 ; locals
        mov [ebp-20],DWORD 0 ;counter      
        mov [ebp-4], DWORD 0 ;carry
        mov [ebp-8], DWORD esi; malloced array
        add [ebp-8], DWORD 4

        start_of_first:
        cmp [ebp-20], edx
        je end_of_first
        push DWORD [eax]
        pop DWORD [ebp-12]
        and [ebp-12],DWORD 0xff

        mov edi,DWORD [ebx]
        mov [ebp-16],DWORD edi
        and [ebp-16], DWORD 0xff
        mov edi, DWORD [ebp-16]
        
        add [ebp-12], edi
        push eax
        push DWORD [ebp-12]
        mov edi, DWORD [ebp-8]
        mov eax, DWORD [edi]
        pop DWORD [edi]
        add [edi],eax
        pop eax


        inc DWORD eax
        inc DWORD ebx
        inc DWORD [ebp-20]
        inc DWORD [ebp-8]
        jmp start_of_first


        end_of_first:

        start_of_sec:
        cmp [ebp-20], ecx
        je end_of_sec
        push DWORD [eax]
        pop DWORD [ebp-12]
        and [ebp-12],DWORD 0xff

        push eax
        push DWORD [ebp-12]
        mov edi, DWORD [ebp-8]
        mov eax, DWORD [edi]
        pop DWORD [edi]
        add [edi],eax
        pop eax


        inc DWORD eax
        inc DWORD [ebp-20]
        inc DWORD [ebp-8]
        jmp start_of_sec


        end_of_sec:


        
        push esi
        call print_multi
        add esp,4
        pushad
        push esi
        call free
        add esp,4
        popad    ;free the malloc

        mov esp,ebp
        pop ebp
        ret

    print_multi:
        push ebp
        mov ebp, esp
        mov eax, DWORD [ebp+8]
        mov ebx, DWORD [eax]
        add eax, 3
        add eax, ebx
        mov ecx, 0
        start_loop:
            mov edx,DWORD 0xff
            and edx, [eax] 
            pushad
            push DWORD edx
            push size
            call printf
            add esp, 8
            popad
            inc ecx 
            dec eax
            cmp ecx, ebx
            jne start_loop
        mov esp,ebp
        pop ebp
        ret

    Get_MaxMin:
		push ebp
		mov ebp, esp
		mov ecx, [eax]
		mov edx, [ebx]
		cmp ecx, edx
		jge else
		mov ecx, eax
		mov eax, ebx
		mov ebx, ecx
		else:
		mov esp,ebp
		pop ebp
		ret


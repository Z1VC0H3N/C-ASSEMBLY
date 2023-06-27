SECTION .data
    argv db "the argv is %s",0 
    argc db "the argc is %d",0
    size db "%x",10,0

SECTION .text
    extern printf
    global main
    x_struct: dd 5
    x_num: db 0xaa, 1,2,0x44,0x4f
    main:
        push ebp
        mov ebp, esp
        pushad
        push x_struct
        call print_multi
        add esp,4 
        popad
        mov esp,ebp
        pop ebp
        ret

    print_multi:
        push ebp
        mov ebp, esp
        mov eax, DWORD [ebp+8]
	pushad
            push DWORD eax
            push size
            call printf
            add esp, 8
            popad
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





        



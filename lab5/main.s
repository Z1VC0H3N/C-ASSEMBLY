SECTION .data
    error db "%x",10,0
    equal db "%c",0
    WRITE EQU 4
    STDOUT EQU 1
    plus dd 0x0000652b
    input dd 0x0000692d
    output dd 0x00006f2d
    newLine dd 0x0000000a
    r db "r",0
SECTION .text
    extern printf
    global main
    main:

    ;ecx = argc and run over key array
    ;edx = argv
    ;ebx = encryption check

        push ebp
        mov ebp, esp
        mov ecx, DWORD [ebp+8]
        mov edx, DWORD [ebp+12]
        add edx,4
        mov esi, DWORD 0
        mov eax, DWORD 1 ; counter
        sub esp,40
        mov [ebp-32],DWORD 0
        mov [ebp-40],DWORD 1
        mov [ebp-28],DWORD 0
        mov [ebp-20], DWORD 0
        start_loop:
        cmp eax, ecx
        je end_loop
            mov ebx, [edx]
            mov edi, [ebx]
            and edi, 0x0000ffff

            cmp DWORD edi, [plus]
            jne end_e_if

            pushad
            mov ecx, DWORD ebx
            call my_strlen
            mov [ebp-16], DWORD eax ; size of the key array
            sub [ebp-16], DWORD 3
            popad
            mov esi,1
            mov [ebp-4], ebx
            add [ebp-4],DWORD 2 ;pointer to the key array.

            end_e_if:
            cmp DWORD edi, [input]
            jne end_i_if
            mov [ebp-20], DWORD 1
            mov [ebp-24], DWORD ebx
            add [ebp-24],DWORD 2
            end_i_if:
            cmp DWORD edi, [output]
            jne end_o_if
            mov [ebp-32],DWORD 1
            mov [ebp-36],DWORD ebx
            add [ebp-36],DWORD 2
            end_o_if:
            add edx,4
            inc eax
            jmp start_loop
        end_loop:
        
        mov ecx,DWORD 0
        cmp [ebp-20], DWORD 0
        je open_out 
        pushad
        push DWORD 0 
        push DWORD 0
        push DWORD [ebp-24]
        push DWORD 5 
        call system_call 
        add esp, 16
        mov [ebp-28], eax
        popad

        open_out:
        cmp [ebp-32], DWORD 0
        je start_func
        pushad
        push DWORD 438
        push DWORD 577
        push DWORD [ebp-36]
        push DWORD 5
        call system_call
        add esp,16
        mov [ebp-40],eax
        popad

        start_func:
        pushad
        push DWORD 1
        push DWORD[ebp-8]
        push DWORD [ebp-28]
        push DWORD 3
        call system_call
        add esp,16
        mov [ebp-12], DWORD eax ; error message
        popad  ;read char from file




        
        cmp [ebp-12], DWORD 0
        je end_func 
        mov eax, DWORD [ebp-8]; read char
        mov ebx, DWORD [ebp-8]
        cmp esi,0
        je not_key
        mov edx,DWORD[ebp-4]
        add edx, ecx ; edx = *[ebp-4][ecx]
        mov edx,[edx]
        and edx, 0xff
        sub edx, DWORD 48

        mov ebx,[eax]
        and ebx, 0xff
        cmp ebx, [newLine]
        je start_func
        add ebx,edx
        mov [eax], ebx

        inc ecx
        cmp ecx,[ebp-16]
        jne not_key
        mov ecx, DWORD 0


        not_key:
        pushad ;printing
        push DWORD 1
        push DWORD eax
        push DWORD [ebp-40]
        push DWORD 4
        call system_call
        add esp,16  
        popad
        jmp start_func
        end_func:
        cmp [ebp-12], DWORD 0
        je end_closeIn 
        pushad
        push DWORD 0 
        push DWORD 1
        push DWORD [ebp-28]
        push DWORD 6 
        call system_call 
        add esp, 16
        mov [ebp-28], eax
        popad
        end_closeIn:
        mov esp,ebp
        pop ebp
        ret


	my_puts: push ebp
	mov ebp, esp
	pushad
	mov ecx, [ebp+8] ; Get first argument p
	call my_strlen
	mov ecx, [ebp+8] ; Get first argument
	mov edx, eax ; Count of bytes
	mov ebx, STDOUT
	mov eax, WRITE
	int 0x80 ; Linux system call
	popad
	mov esp, ebp
	pop ebp
	ret

	my_strlen: mov eax,1
	cont: cmp byte [ecx], 0
	jz done
	inc ecx
	inc eax
	jmp cont
	done: ret

system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

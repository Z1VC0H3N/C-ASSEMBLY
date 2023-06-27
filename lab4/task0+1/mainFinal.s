; FOR LOOP: Print commandline arguments> an equivalent program to 
;this in assembly
SECTION .data

    argv db "Arguments = %s",10,0
    argc db "Argument Count = %d",10,0

SECTION .text
; allow access to printf
extern printf
; make main_function available externally
global main


main:    ; int main (int argc, char* argv[])
    push ebp
    mov ebp,esp
    sub esp, 10

    mov eax, DWORD [ebp + 8]    ; points to argc
    mov ebx, DWORD [ebp + 12]   ; points to argv
    mov ecx, 0      ; mov ZERO to count register

begin_for_loop:
    ; always preserve values prior to external function calls
    ; external function calls may modify values you placed in registers
    push ebx    ; preserve ebx; holds argument address
    push eax    ; preserve eax; holds number of arguments
    push ecx    ; preserve ecx; holds the counter

    ; call printf
    push DWORD [ebx]
    push argv
    call printf
    add esp, 8  ; clean up the stack

    ; always restore in backwards order
    pop ecx     ; restore counter
    pop eax     ; restore number of arguments
    pop ebx     ; restore argument address

    inc ecx     ; increase our counter by 1
    add ebx, 4  ; move to next argument in the array

    ; the loop
    cmp ecx, eax        ; if ecx not equal to eax
    jne begin_for_loop  ; jmp to loop, else exit


    mov esp,ebp
    pop ebp
    ret

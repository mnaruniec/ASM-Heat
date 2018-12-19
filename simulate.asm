global start
global step

section .bss
    WIDTH resb 4
    HEIGHT resb 4
    TRUE_WIDTH resb 4
    WEIGHT resb 4
    MATRIX resb 8
    DELTA resb 8


section .text

start:
    ; edi=width, esi=height, rdx=*matrix, rcx=*heaters, r8=*coolers, xmm0=weight
    mov [WIDTH], edi
    lea rcx, [rdi + 4]
    mov [TRUE_WIDTH], ecx
    mov [HEIGHT], esi
    mov [MATRIX], rdx
    movss dword [WEIGHT], xmm0

    add rsi, 2
    imul rcx, rsi
    lea rdx, [rdx + 4 * rcx]
    mov [DELTA], rdx ; DELTA = MEDIUM + 4 * TRUE_WIDTH * (HEIGHT + 2)
    ret

step:
    mov eax, [HEIGHT]
    mov ecx, [WIDTH]
    sal rcx, 2
    mov edx, [TRUE_WIDTH]
    sal rdx, 2


    mov rsi, [MATRIX] ; current element ptr
    lea rsi, [rsi + rdx + 4] ; first element


    mov rdi, [DELTA] ; current element's delta ptr
    lea rdi, [rdi + 4]

    lea r8, [rsi + rcx] ; behind-last element ptr in current row

    mov r9, 0 ; current row

    movss xmm2, dword [WEIGHT]

process_elem_delta:
    movss xmm1, dword [rsi]
    shufps xmm1, xmm1, 0 ; broadcast current element onto xmm1

    sub rsi, 4 ; go to left neighbour
    movups xmm0, [rsi] ; xmm0: trash;right;elem;left

    sub rsi, rdx
    add rsi, 4 ; go to top neighbour
    shufps xmm0, xmm0, 11100001b ; xmm0: trash;right;left;elem
    movss xmm3, dword [rsi]
    movss xmm0, xmm3 ; xmm0: trash;right;left;top

    lea rsi, [rsi + 2 * rdx] ; go to bottom neighbour
    shufps xmm0, xmm0, 00100111b ; xmm0: top;right;left;trash
    movss xmm3, dword [rsi]
    movss xmm0, xmm3 ; xmm0: top;right;left;bottom

    subps xmm0, xmm1
    haddps xmm0, xmm0
    haddps xmm0, xmm0 ; sum differences from each neighbour
    mulss xmm0, xmm2 ; multiply by weight

    movss dword [rdi], xmm0 ; move result to current element's delta

    sub rsi, rdx ; go back to current element

next_elem_delta:
    add rsi, 4
    add rdi, 4

    cmp rsi, r8
    jb process_elem_delta ; if (curr_ptr < behind_last_ptr) do not change row

next_row_delta:
    add rsi, 4 * 4 ; skip 2 terminating guards and left cooler
    add rdi, 4 * 4
    lea r8, [rsi + rcx] ; set new behind-last element ptr

    inc r9
    cmp r9, rax
    jl process_elem_delta ; if (curr_row < height) continue the loop

prepare_result:
    ret
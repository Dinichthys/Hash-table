section .data

kWordLen     equ 32
kSizePointer equ 8
kSizeSizeT   equ 8
kEndSym      equ 0
kMask        equ 0xFFFFFFFF
kMaskAlign   equ 0b11111
kAlignment   equ 32

kPoison           equ -1
kTooLongWordError equ -2

;--------------------------------------------
section .text

global ASMListFindElemSIMD


ASMListFindElemSIMD:
    push rbp
    mov rbp, rsp

    mov rax, rsp
    and rax, kMaskAlign
    add rsp, rax
    sub rsp, kAlignment

    push kEndSym
    push kEndSym
    push kEndSym
    push kEndSym                            ; Place in stack for buffer

    mov rcx, kWordLen

.WriteWordToStack_Comparison:
    cmp byte [rsi], kEndSym
    je .WriteWordToStack_Stop

    test rcx, rcx
    je .WriteWordToStack_Error

.WriteWordToStack_Body:
    mov al, byte [rsi]
    mov byte [rsp], al
    inc rsp
    inc rsi
    dec rcx
    jmp .WriteWordToStack_Comparison
.WriteWordToStack_Stop:                    ; The word was written to the stack

    mov rsi, rcx
    mov rcx, kWordLen
    sub rcx, rsi
    sub rsp, rcx                           ; Set RSP on the start of the word

    vmovaps ymm0, yword [rsp]              ; YMM0 = The word
    mov rcx, qword [rdi + kSizePointer]    ; RCX = array of orders in the list
    mov rdx, qword [rcx]                   ; RDX = Index of the element
    mov rdi, qword [rdi]                   ; RDI = array of data

.FindElemInList_Comparison:

    test rdx, rdx                          ; RDX = Null element
    je .WordNotFound

    mov r8, rdx
    shl r8, 6                              ; R8 = Index * 64 ((sizeof (hash_elem_t)))

    mov r9, rdi
    add r9, r8                             ; R9 = Current word

    vmovaps ymm1, yword [r9]
    vpxor ymm2, ymm1, ymm0                 ; Comparison
    vpmovmskb eax, ymm2                    ; Move mask
    cmp eax, kMask                         ; Check that all of 32 bits are set correctly (0xFFFFFFFF)

    jne .FindElemInList_Continue

.FindElemInList_Body:
    mov rsp, rbp
    pop rbp
    mov rax, rdx
    ret

.FindElemInList_Continue:
    mov r8, rdx
    shl r8, 4                              ; R8 = Index * 16 (sizeof (order_list_t))
    mov r9, rcx
    add r9, r8                             ; R9 = Current element
    mov rdx, qword [r9]
    jmp .FindElemInList_Comparison

.WordNotFound:
    mov rsp, rbp
    pop rbp
    mov rax, kPoison
    ret

.WriteWordToStack_Error:
    mov rsp, rbp
    pop rbp
    mov rax, kTooLongWordError
    ret

; signed long long ListFindElemSIMD (const list_t* const list, const char* const element)
; {
;     ASSERT (list    != NULL, "Invalid pointer for list for ListFindElem\n");
;     ASSERT (element != NULL, "Invalid pointer for element ListFindElem\n");
;
;     LOG (kDebug, "Looking for \"%s\"\n", element);
;
;     char element_str [kMaxWordLen] = "";
;     strcpy (element_str, element);
;
;     __m256i element_SIMD = _mm256_load_si256 ((const __m256i*) element_str);
;
;     size_t list_elem_index = list->order[0].next;
;
;     hash_elem_t* hash_elem_arr = (hash_elem_t*)(list->data);
;
;     __m256i cmp_elem = _mm256_setzero_si256 ();
;     while (list_elem_index != 0)
;     {
;         cmp_elem = _mm256_load_si256 ((const __m256i*) hash_elem_arr[list_elem_index].string);
;         if (_mm256_movemask_epi8 (_mm256_cmpeq_epi32 (element_SIMD, cmp_elem)) == 0xFF'FF'FF'FF)
;         {
;             LOG (kDebug, "Found \"%s\"\n", element);
;             return (signed long long) list_elem_index;
;         }
;         list_elem_index = list->order[list_elem_index].next;
;     }
;
;     LOG (kDebug, "Didn't found \"%s\"\n", element);
;
;     return kPoisonVal;
; }

section .note.GNU-stack

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
    mov rcx, qword [rdi + 2*kSizePointer]  ; RCX = number of list elements
    mov rdi, qword [rdi]                   ; RDI = array of data
    mov rdx, 1

    mov r9, rdi
    add r9, 2 * kWordLen                   ; R9 = Current word

    inc rcx
    vmovaps ymm2, yword [r9]

.FindElemInList_Comparison:

    cmp rdx, rcx                          ; RDX = Null element
    jae .WordNotFound

    inc rdx

    vmovaps ymm1, ymm2
    add r9, 2 * kWordLen                  ; R9 = Current word

    vmovaps ymm2, yword [r9]
    vpxor ymm3, ymm1, ymm0                ; Comparison
    vptest ymm3, ymm3                     ; Check that ymm2 == 0

    jne .FindElemInList_Comparison

.FindElemInList_Stop:
    mov rsp, rbp
    pop rbp
    mov rax, rdx
    mov rdi, rdx
    ret

.WordNotFound:
    mov rsp, rbp
    pop rbp
    mov rax, kPoison
    mov rdi, rax
    ret

.WriteWordToStack_Error:
    mov rsp, rbp
    pop rbp
    mov rax, kTooLongWordError
    mov rdi, rax
    ret

; What does this function do:
;
; int64_t ListFindElemSIMD (const list_t* const list, const char* const element)
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
;             return (int64_t) list_elem_index;
;         }
;         list_elem_index = list->order[list_elem_index].next;
;     }
;
;     LOG (kDebug, "Didn't found \"%s\"\n", element);
;
;     return kPoisonVal;
; }

section .note.GNU-stack

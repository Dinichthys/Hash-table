#include "HashTable/hash_table_find.h"
#include "HashTable/asm_list_find_elem_simd.h"
#include "HashTable/hash_table.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "List/include/list.h"

#include "MyLib/Assert/my_assert.h"
#include "MyLib/Logger/logging.h"

signed long long HashTableFindElem (hash_table_t hash_table, const char* const element)
{
    ASSERT (hash_table != NULL, "Invalid pointer for hash table for HashTableFindElem\n");
    ASSERT (element    != NULL, "Invalid pointer for element HashTableFindElem\n");

    const size_t bucket_index = Hashing (element, strlen (element)) % kNumBucket;

    const signed long long val_index =
    ListFindElem (&hash_table [bucket_index], element);

    if (val_index == kPoisonVal)
    {
        return kPoisonVal;
    }

    return (signed long long) ((hash_elem_t*)(hash_table [bucket_index].data))[val_index].counter;
}

signed long long ListFindElem (const list_t* const list, const char* const element)
{
    ASSERT (list    != NULL, "Invalid pointer for list for ListFindElem\n");
    ASSERT (element != NULL, "Invalid pointer for element ListFindElem\n");

    LOG (kDebug, "Looking for \"%s\"\n", element);

    size_t list_elem_index = list->order[0].next;

    hash_elem_t* hash_elem_arr = (hash_elem_t*)(list->data);

    while (list_elem_index != 0)
    {
        if (strcmp (hash_elem_arr[list_elem_index].string, element) == 0)
        {
            LOG (kDebug, "Found \"%s\"\n", element);
            return (signed long long) list_elem_index;
        }
        list_elem_index = list->order[list_elem_index].next;
    }

    LOG (kDebug, "Didn't found \"%s\"\n", element);

    return kPoisonVal;
}

signed long long HashTableFindElemSIMD (hash_table_t hash_table, const char* const element)
{
    ASSERT (hash_table != NULL, "Invalid pointer for hash table for HashTableFindElem\n");
    ASSERT (element    != NULL, "Invalid pointer for element HashTableFindElem\n");

    const size_t bucket_index = Hashing (element, strlen (element)) % kNumBucket;

    const signed long long val_index =
    ListFindElemSIMD (&hash_table [bucket_index], element);

    if (val_index == kPoisonVal)
    {
        return kPoisonVal;
    }

    return (signed long long) ((hash_elem_t*)(hash_table [bucket_index].data))[val_index].counter;
}

signed long long ListFindElemSIMD (const list_t* const list, const char* const element)
{
    ASSERT (list    != NULL, "Invalid pointer for list for ListFindElem\n");
    ASSERT (element != NULL, "Invalid pointer for element ListFindElem\n");

    LOG (kDebug, "Looking for \"%s\"\n", element);

    char element_str [kMaxWordLen] = "";
    strcpy (element_str, element);

    __m256i element_SIMD = _mm256_load_si256 ((const __m256i*) element_str);

    size_t list_elem_index = list->order[0].next;

    hash_elem_t* hash_elem_arr = (hash_elem_t*)(list->data);

    __m256i cmp_elem = _mm256_setzero_si256 ();
    while (list_elem_index != 0)
    {
        cmp_elem = _mm256_load_si256 ((const __m256i*) hash_elem_arr[list_elem_index].string);
        if (_mm256_movemask_epi8 (_mm256_cmpeq_epi32 (element_SIMD, cmp_elem)) == 0xFF'FF'FF'FF)
        {
            LOG (kDebug, "Found \"%s\"\n", element);
            return (signed long long) list_elem_index;
        }
        list_elem_index = list->order[list_elem_index].next;
    }

    LOG (kDebug, "Didn't found \"%s\"\n", element);

    return kPoisonVal;
}

signed long long ASMHashTableFindElemSIMD (hash_table_t hash_table, const char* const element)
{
    ASSERT (hash_table != NULL, "Invalid pointer for hash table for HashTableFindElem\n");
    ASSERT (element    != NULL, "Invalid pointer for element HashTableFindElem\n");

    const size_t bucket_index = Hashing (element, strlen (element)) % kNumBucket;

    ASMListFindElemSIMD (&hash_table [bucket_index], element);

    asm(//".intel_syntax noprefix\n\t"
        "movq %%rdi, %%rax\n\t"
        "cmpq $0xFFFFFFFFFFFFFFFF, %%rax\n\t"
        "je .SkipASM\n\t"

        "movq $32, %%r8\n\t"
        "movq (%%r9,%%r8,1), %%rax\n\t"

        ".SkipASM:\n\t"
        "addq $8, %%rsp\n\t"
        "pop %%rbx\n\t"
        "pop %%rbp\n\t"
        "ret\n\t"
        :
        : "r" (kPoisonVal)
        :
    );
}

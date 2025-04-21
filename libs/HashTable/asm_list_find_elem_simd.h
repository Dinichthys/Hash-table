#ifndef ASM_LIST_FIND_ELEM_SIMD_H
#define ASM_LIST_FIND_ELEM_SIMD_H

#include "HashTable/hash_table.h"

#include <stdlib.h>

#include "List/include/list.h"

extern "C" int64_t ASMListFindElemSIMD (const list_t* const list, const char* const element);

#endif // ASM_LIST_FIND_ELEM_SIMD_H

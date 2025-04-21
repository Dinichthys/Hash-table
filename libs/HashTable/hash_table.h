#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>

#include "List/include/list.h"

static const size_t kNumBucket = 2'000;
static const size_t kMaxWordLen = 32;

enum HashTableError
{
    kDoneHashTable = 0,

    kCantCreateList                      = 1,
    kCantPushElemListHashTable           = 2,

    kCantDumpHashTable                   = 3,

    kVerifyHashTableListNullPtr          = 7,
    kVerifyHashTableListInvalidZeroElem  = 8,
    kVerifyHashTableListInvalidSize      = 9,
    kVerifyHashTableListInvalidOrderList = 10,
    kVerifyHashTableListInvalidOrderFree = 11,
};

typedef struct hash_elem
{
    alignas (__m256) char string [kMaxWordLen];
    size_t counter;
} hash_elem_t;

typedef list_t bucket_t;

typedef bucket_t hash_table_t [kNumBucket];

#include "HashTable/hash_table_construction.h"
#include "HashTable/hash_table_find.h"
#include "HashTable/hash_table_push.h"
#include "HashTable/hash_table_pop.h"
#include "HashTable/hash_table_verify.h"
#include "HashTable/hash.h"

#include "HashTable/asm_list_find_elem_simd.h"

#endif // HASH_TABLE_H

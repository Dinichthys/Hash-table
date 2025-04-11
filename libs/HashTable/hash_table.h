#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>

#include "List/include/list.h"

static const size_t kNumBucket = 5'000;
static const size_t kMaxWordLen = 256;

enum HashTableError
{
    kDoneHashTable = 0,

    kCantCreateList = 1,
    kCantPushElemListHashTable = 2,
};

typedef struct hash_elem
{
    char string [kMaxWordLen];
    size_t counter;
} hash_elem_t;

typedef list_t bucket_t;

typedef bucket_t hash_table_t [kNumBucket];

#include "HashTable/hash_table_construction.h"
#include "HashTable/hash_table_find.h"
#include "HashTable/hash_table_push.h"
#include "HashTable/hash.h"

#endif // HASH_TABLE_H

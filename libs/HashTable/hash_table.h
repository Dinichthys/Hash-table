#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>

#include "List/include/list.h"

enum HashTableError
{
    kDoneHashTable = 0,

    kCantCreateList = 1,
    kCantPushElemListHashTable = 2,
};

typedef struct bucket
{
    list_t strings;
    list_t counters;
} bucket_t;

static const size_t kNumBucket = 5;

typedef bucket_t hash_table_t [kNumBucket];

#include "HashTable/hash_table_construction.h"
#include "HashTable/hash_table_find.h"
#include "HashTable/hash_table_push.h"
#include "HashTable/hash.h"

#endif // HASH_TABLE_H

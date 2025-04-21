#include "HashTable/hash_table_pop.h"
#include "HashTable/hash_table.h"

#include <stdlib.h>
#include <string.h>
#include <immintrin.h>

#include "List/include/list.h"

#include "MyLib/Assert/my_assert.h"
#include "MyLib/Logger/logging.h"

enum HashTableError HashTablePopElem (hash_table_t hash_table, const char* const element)
{
    ASSERT (hash_table != NULL, "Invalid pointer for hash table for HashTableFindElem\n");
    ASSERT (element    != NULL, "Invalid pointer for element HashTableFindElem\n");

    const size_t bucket_index = Hashing (element, strlen (element)) % kNumBucket;

    const int64_t val_index =
    ListFindElem (&hash_table [bucket_index], element);

    if (val_index == kPoisonVal)
    {
        return kDoneHashTable;
    }

    hash_elem_t new_elem = {.string = "", .counter = 1};

    ListPopAfterIndex (&hash_table [bucket_index], &new_elem, val_index);

    return kDoneHashTable;
}

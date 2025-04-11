#include "HashTable/hash_table_push.h"
#include "HashTable/hash_table.h"

#include <stdlib.h>
#include <string.h>

#include "List/include/list.h"

#include "My_lib/Assert/my_assert.h"
#include "My_lib/Logger/logging.h"

enum HashTableError HashTablePushElem (hash_table_t hash_table, const char* const element)
{
    ASSERT (hash_table != NULL, "Invalid pointer for hash table for HashTableFindElem\n");
    ASSERT (element    != NULL, "Invalid pointer for element HashTableFindElem\n");

    const size_t bucket_index = Hashing (element, strlen (element)) % kNumBucket;

    const signed long long val_index =
    ListFindElem (&hash_table [bucket_index], element);

    if (val_index != kPoisonVal)
    {
        hash_elem_t found_elem = {};

        ListElemValLoad (&hash_table [bucket_index], val_index, &found_elem);

        found_elem.counter++;

        ListElemValStor (&hash_table [bucket_index], val_index, &found_elem);

        return kDoneHashTable;
    }

    enum ListError error = kDoneList;

    hash_elem_t new_elem = {.string = element, .counter = 1};

    error = ListPushFront (&hash_table [bucket_index], &new_elem);
    if (error != kDoneList)
    {
        return kCantPushElemListHashTable;
    }

    return kDoneHashTable;
}

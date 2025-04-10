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
    ListFindElem (hash_table [bucket_index].strings, element);

    if (val_index != kPoisonVal)
    {
        size_t counter = 0;

        ListElemValLoad (&hash_table [bucket_index].counters, val_index, &counter);

        counter++;

        ListElemValStor (&hash_table [bucket_index].counters, val_index, &counter);

        return kDoneHashTable;
    }

    enum ListError error = kDoneList;

    error = ListPushFront (&hash_table [bucket_index].strings, &element);
    if (error != kDoneList)
    {
        return kCantPushElemListHashTable;
    }

    size_t counter = 1;

    error = ListPushFront (&hash_table [bucket_index].counters, &counter);
    if (error != kDoneList)
    {
        return kCantPushElemListHashTable;
    }

    return kDoneHashTable;
}

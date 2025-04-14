#include "HashTable/hash_table_push.h"
#include "HashTable/hash_table.h"

#include <stdlib.h>
#include <string.h>

#include "List/include/list.h"

#include "MyLib/Assert/my_assert.h"
#include "MyLib/Logger/logging.h"

enum HashTableError HashTablePushElem (hash_table_t hash_table, const char* const element)
{
    ASSERT (hash_table != NULL, "Invalid pointer for hash table for HashTableFindElem\n");
    ASSERT (element    != NULL, "Invalid pointer for element HashTableFindElem\n");

    const size_t bucket_index = Hashing (element, strlen (element)) % kNumBucket;

    const signed long long val_index =
    ListFindElem (&hash_table [bucket_index], element);

    if (val_index != kPoisonVal)
    {
        ((hash_elem_t*)(hash_table [bucket_index].data))[val_index].counter++;

        return kDoneHashTable;
    }

    enum ListError error = kDoneList;

    hash_elem_t new_elem = {.string = "", .counter = 1};
    strcpy (new_elem.string, element);

    error = ListPushFront (&hash_table [bucket_index], &new_elem);
    if (error != kDoneList)
    {
        return kCantPushElemListHashTable;
    }

    return kDoneHashTable;
}

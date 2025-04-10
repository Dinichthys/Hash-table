#include "HashTable/hash_table_find.h"
#include "HashTable/hash_table.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "List/include/list.h"

#include "My_lib/Assert/my_assert.h"
#include "My_lib/Logger/logging.h"

signed long long HashTableFindElem (hash_table_t hash_table, const char* const element)
{
    ASSERT (hash_table != NULL, "Invalid pointer for hash table for HashTableFindElem\n");
    ASSERT (element    != NULL, "Invalid pointer for element HashTableFindElem\n");

    const size_t bucket_index = Hashing (element, strlen (element)) % kNumBucket;

    const signed long long val_index =
    ListFindElem (hash_table [bucket_index].strings, element);

    if (val_index == kPoisonVal)
    {
        return kPoisonVal;
    }

    size_t counter = 0;

    ListElemValLoad (&hash_table [bucket_index].counters, val_index, &counter);

    return (signed long long) counter;
}

signed long long ListFindElem (const list_t list, const char* const element)
{
    ASSERT (list    != NULL, "Invalid pointer for list for ListFindElem\n");
    ASSERT (element != NULL, "Invalid pointer for element ListFindElem\n");

    size_t list_elem_index = TailIndex (&list);
    char* list_string = NULL;

    while (list_elem_index != 0)
    {
        ListElemValLoad (&list, list_elem_index, &list_string);
        if (strcmp (list_string, element) == 0)
        {
            return (signed long long) list_elem_index;
        }
    }

    return kPoisonVal;
}

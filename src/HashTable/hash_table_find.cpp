#include "HashTable/hash_table_find.h"
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

    hash_elem_t hash_elem = {};

    ListElemValLoad (&hash_table [bucket_index], val_index, &hash_elem);

    return (signed long long) hash_elem.counter;
}

signed long long ListFindElem (const list_t* const list, const char* const element)
{
    ASSERT (list    != NULL, "Invalid pointer for list for ListFindElem\n");
    ASSERT (element != NULL, "Invalid pointer for element ListFindElem\n");

    LOG (kDebug, "Looking for \"%s\"\n", element);

    size_t list_elem_index = TailIndex (list);
    hash_elem_t cur_elem = {};

    while (list_elem_index != 0)
    {
        ListElemValLoad (list, list_elem_index, &cur_elem);
        if (strcmp (cur_elem.string, element) == 0)
        {
            LOG (kDebug, "Found \"%s\"\n", element);
            return (signed long long) list_elem_index;
        }
        list_elem_index = NextIndex (list, list_elem_index);
    }

    LOG (kDebug, "Didn't found \"%s\"\n", element);

    return kPoisonVal;
}

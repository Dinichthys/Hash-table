#include "HashTable/hash_table_construction.h"
#include "HashTable/hash_table.h"

#include <stdlib.h>

#include "List/include/list.h"

#include "MyLib/Assert/my_assert.h"
#include "MyLib/Logger/logging.h"

enum HashTableError HashTableCtor (hash_table_t* hash_table)
{
    ASSERT (hash_table != NULL, "Invalid pointer for HashTableCtor\n");

    enum ListError error = kDoneList;
    for (size_t bucket_index = 0; bucket_index < kNumBucket; bucket_index++)
    {
        error = ListCtor (&((*hash_table) [bucket_index]),  kStartNumListElem, sizeof (hash_elem_t));
        if (error != kDoneList)
        {
            for (; bucket_index > 0; bucket_index--)
            {
                ListDtor (&(*hash_table) [bucket_index - 1]);
            }
            return kCantCreateList;
        }
    }

    return kDoneHashTable;
}

enum HashTableError HashTableDtor (hash_table_t* hash_table)
{
    ASSERT (hash_table != NULL, "Invalid pointer for HashTableCtor\n");

    for (size_t bucket_index = 0; bucket_index < kNumBucket; bucket_index++)
    {
        ListDtor (&(*hash_table) [bucket_index]);
    }

    return kDoneHashTable;
}

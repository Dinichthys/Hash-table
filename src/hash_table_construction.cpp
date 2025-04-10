#include "HashTable/hash_table_construction.h"
#include "HashTable/hash_table.h"

#include <stdlib.h>

#include "List/include/list.h"

#include "My_lib/Assert/my_assert.h"
#include "My_lib/Logger/logging.h"

enum HashTableError HashTableCtor (hash_table_t* hash_table)
{
    ASSERT (hash_table != NULL, "Invalid pointer for HashTableCtor\n");

    enum ListError error = kDoneList;
    for (size_t bucket_index = 0; bucket_index < kNumBucket; bucket_index++)
    {
        error = ListCtor (&((*hash_table) [bucket_index].strings),  kStartNumListElem, sizeof (char*));
        if (error != kDoneList)
        {
            for (; bucket_index > 0; bucket_index--)
            {
                ListDtor (&(*hash_table) [bucket_index - 1].strings );
                ListDtor (&(*hash_table) [bucket_index - 1].counters);
            }
            return kCantCreateList;
        }
        error = ListCtor (&(*hash_table) [bucket_index].counters, kStartNumListElem, sizeof (size_t));
        if (error != kDoneList)
        {
            ListDtor (&(*hash_table) [bucket_index].strings);
            for (; bucket_index > 0; bucket_index--)
            {
                ListDtor (&(*hash_table) [bucket_index - 1].strings );
                ListDtor (&(*hash_table) [bucket_index - 1].counters);
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
        ListDtor (&(*hash_table) [bucket_index].strings );
        ListDtor (&(*hash_table) [bucket_index].counters);
    }

    return kDoneHashTable;
}

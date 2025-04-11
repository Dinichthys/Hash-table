#include "HashTable/hash.h"

#include <stdlib.h>

#include "MyLib/Assert/my_assert.h"
#include "MyLib/Logger/logging.h"

size_t Hashing (const char* const val, const size_t num)
{
    ASSERT (val != NULL, "Invalid pointer for hashing\n");

    const char* hash_ptr = val;

    size_t hash_sum = 0;

    while (hash_ptr < val + num)
    {
        hash_sum += (size_t) *hash_ptr;
        hash_ptr++;
    }

    return hash_sum;
}

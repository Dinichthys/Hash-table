#include "HashTable/strcmp.h"

#include <immintrin.h>

#include "MyLib/Assert/my_assert.h"
#include "MyLib/Logger/logging.h"

int strcmp_256 (const char* const first_str, const char* const second_str)
{
    ASSERT (first_str != NULL, "First string is null pointer in function strcmp\n");
    ASSERT (second_str != NULL, "Second string is null pointer in function strcmp\n");

    __m256i first_m256  = _mm256_load_si256 ((__m256i*) first_str);
    __m256i second_m256 = _mm256_load_si256 ((__m256i*) second_str);

    if (_mm256_movemask_epi8 (_mm256_cmpeq_epi32 (first_m256, second_m256)) == 0xFF'FF'FF'FF)
    {
        return 0;
    }

    return kNEqual;
}

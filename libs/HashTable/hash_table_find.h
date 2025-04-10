#ifndef HASH_TABLE_FIND_H
#define HASH_TABLE_FIND_H

#include "HashTable/hash_table.h"

#include <stdlib.h>

#include "List/include/list.h"

static const signed long long kPoisonVal = -1;

signed long long HashTableFindElem (hash_table_t hash_table, char const* const element);
signed long long ListFindElem      (const list_t list, const char* const element);

#endif // HASH_TABLE_FIND_H

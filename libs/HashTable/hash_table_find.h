#ifndef HASH_TABLE_FIND_H
#define HASH_TABLE_FIND_H

#include "HashTable/hash_table.h"

#include <stdlib.h>

#include "List/include/list.h"

static const int64_t kPoisonVal = -1;

int64_t HashTableFindElem (hash_table_t hash_table, char const* const element);
int64_t ListFindElem      (const list_t* const list, const char* const element);

int64_t HashTableFindElemStrCmp (hash_table_t hash_table, char const* const element);
int64_t ListFindElemStrCmp      (const list_t* const list, const char* const element);

int64_t HashTableFindElemSIMD (hash_table_t hash_table, char const* const element);
int64_t ListFindElemSIMD      (const list_t* const list, const char* const element);

int64_t ASMHashTableFindElemSIMD (hash_table_t hash_table, char const* const element);

int64_t InlineASMHashTableFindElemSIMD (hash_table_t hash_table, char const* const element);

#endif // HASH_TABLE_FIND_H

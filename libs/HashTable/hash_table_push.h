#ifndef HASH_TABLE_PUSH_H
#define HASH_TABLE_PUSH_H

#include "HashTable/hash_table.h"

#include <stdlib.h>

#include "List/include/list.h"

enum HashTableError HashTablePushElem     (hash_table_t hash_table, const char* const element);
enum HashTableError HashTablePushElemSIMD (hash_table_t hash_table, char* const element);

#endif // HASH_TABLE_PUSH_H

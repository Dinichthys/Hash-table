#ifndef HASH_TABLE_CONSTRUCTION_H
#define HASH_TABLE_CONSTRUCTION_H

#include "HashTable/hash_table.h"

#include <stdlib.h>

#include "List/include/list.h"

static const size_t kStartNumListElem = 20;

enum HashTableError HashTableCtor (hash_table_t* hash_table);
enum HashTableError HashTableDtor (hash_table_t* hash_table);

#endif // HASH_TABLE_CONSTRUCTION_H

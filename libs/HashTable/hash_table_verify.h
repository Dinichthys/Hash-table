#ifndef HASH_TABLE_VERIFY_H
#define HASH_TABLE_VERIFY_H

#include "HashTable/hash_table.h"

enum HashTableError HashTableVerify (const hash_table_t* const hash_table);
enum HashTableError HashTableDump   (const hash_table_t* const hash_table);

#endif // HASH_TABLE_VERIFY_H

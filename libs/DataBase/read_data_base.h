#ifndef READ_DATA_BASE_H
#define READ_DATA_BASE_H

#include <stdlib.h>
#include <stdio.h>

#include "HashTable/hash_table.h"

enum DataBaseError
{
    kDoneDataBase = 0,

    kCantCallocBufferDataBase    = 1,
    kCantReadFileDataBase        = 2,
    kCantCallocWordArrayDataBase = 3,
    kCantPushElemListDataBase    = 4,
};

typedef char word_t [kMaxWordLen];

enum DataBaseError ReadDataBase (FILE* const data_base, hash_table_t* const hash_table);

#endif // READ_DATA_BASE_H

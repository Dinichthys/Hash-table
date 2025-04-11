#include "DataBase/read_data_base.h"

#include <stdio.h>
#include <ctype.h>
#include <memory.h>

#include "HashTable/hash_table.h"

#include "MyLib/Assert/my_assert.h"
#include "MyLib/Logger/logging.h"
#include "MyLib/My_stdio/my_stdio.h"

static enum DataBaseError ReadToBuffer   (FILE* const data_base, char* const buffer);
static size_t             CountLetters   (const char* const buffer);
static size_t             SkipNotLetters (const char* const buffer);

enum DataBaseError ReadDataBase (FILE* const data_base, hash_table_t* const hash_table)
{
    ASSERT (data_base != NULL, "Invalid argument data_base for ReadDataBase.\n");

    size_t buffer_len = size_of_file (data_base);
    char* buffer = (char*) calloc (buffer_len + 1, sizeof (char));
    if (buffer == NULL)
    {
        return kCantCallocBufferDataBase;
    }

    word_t* word_array = (word_t*) calloc (buffer_len, sizeof (word_t));
    if (word_array == NULL)
    {
        free (buffer);
        return kCantCallocWordArrayDataBase;
    }

    if (ReadToBuffer (data_base, buffer) != kDoneDataBase)
    {
        free (word_array);
        free (buffer);
        return kCantReadFileDataBase;
    }

    size_t word_index = 0;
    size_t word_len = 0;
    size_t num_sym_done = 0;

    while (num_sym_done < buffer_len)
    {
        num_sym_done += SkipNotLetters (buffer + num_sym_done);
        word_len = CountLetters (buffer + num_sym_done);

        if (word_len < kMaxWordLen)
        {
            memcpy (word_array [word_index], buffer + num_sym_done, word_len);
            word_array [word_index] [word_len] = '\0';
            word_index++;
            LOG (kDebug, "Word index = %d\n", word_index);
        }

        num_sym_done += word_len;
    }
    word_array [word_index] [0] = '\0';

    while (word_index > 0)
    {
        word_index--;
        LOG (kDebug, "Word index for pushing = %d\n", word_index);
        if (HashTablePushElem (*hash_table, word_array [word_index]) != kDoneHashTable)
        {
            free (word_array);
            free (buffer);
            return kCantPushElemListDataBase;
        }
    }

    free (word_array);
    free (buffer);

    return kDoneDataBase;
}

static enum DataBaseError ReadToBuffer (FILE* const data_base, char* const buffer)
{
    ASSERT (data_base != NULL, "Invalid argument data_base for ReadDataBase.\n");
    ASSERT (buffer    != NULL, "Invalid argument buffer for ReadDataBase.\n");

    size_t file_len = size_of_file (data_base);
    size_t cur_num_sym = fread (buffer, sizeof(char), file_len, data_base);
    if (file_len != cur_num_sym)
    {
        return kCantReadFileDataBase;
    }

    buffer [file_len] = '\0';

    return kDoneDataBase;
}

static size_t CountLetters (const char* const buffer)
{
    size_t counter = 0;

    while (isalpha (*(buffer + counter)))
    {
        counter++;
    }

    return counter;
}

static size_t SkipNotLetters (const char* const buffer)
{
    size_t counter = 0;

    while ((*(buffer + counter) != '\0') && (!isalpha (*(buffer + counter))))
    {
        counter++;
    }

    return counter;
}

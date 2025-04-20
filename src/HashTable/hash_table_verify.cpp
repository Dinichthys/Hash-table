#include "HashTable/hash_table_verify.h"
#include "HashTable/hash_table.h"

#include <stdlib.h>
#include <stdio.h>

#include "MyLib/Assert/my_assert.h"
#include "MyLib/Logger/logging.h"

void PrintData (void* const data, FILE* const stream);

enum HashTableError HashTableVerify (const hash_table_t* const hash_table)
{
    ASSERT (hash_table != NULL, "Invalid argument hash_table for function HashTableVerify\n");

    enum ListError error = kDoneList;

    for (size_t bucket_index = 0; bucket_index < kNumBucket; bucket_index++)
    {
        error = VerifyList (*hash_table + bucket_index);
        if (error != kDoneList)
        {
            return (HashTableError) error;
        }
    }

    return (HashTableError) error;
}

enum HashTableError HashTableDump (const hash_table_t* const hash_table)
{
    ASSERT (hash_table != NULL, "Invalid argument hash_table for function HashTableVerify\n");

    FILE* dump_file_html = fopen ("Dump_Files/Dump.html", "w");
    if (dump_file_html == NULL)
    {
        return kCantDumpHashTable;
    }

    fprintf (dump_file_html, "<pre>\n"
                             "<HR>\n"
                             "В хеш таблице имеется %lu бакетов.\n"
                             "Их дампы будут перечислены ниже\n\n", kNumBucket);

    fclose (dump_file_html);

    for (size_t bucket_index = 0; bucket_index < kNumBucket; bucket_index++)
    {
        ListDump(*hash_table + bucket_index, PrintData);
    }

    return kDoneHashTable;
}

void PrintData (void* const data, FILE* const stream)
{
    ASSERT (data   != NULL, "Invalid argument data for PrintData in HashTableDump\n");
    ASSERT (stream != NULL, "Invalid argument stream for PrintData in HashTableDump\n");

    fprintf (stream, "| string | %s | counter | %lu ", (char*) (((hash_elem_t*) data)->string), ((hash_elem_t*) data)->counter);
}

#include "HashTable/hash_table.h"
#include "DataBase/read_data_base.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "MyLib/Logger/logging.h"

int main()
{
    set_log_lvl (kDebug);

    hash_table_t hash_table = {};
    HashTableCtor (&hash_table);

    time_t start = time (NULL);

    FILE* const data_base = fopen ("DataBase/HarryPotter.txt", "r");
    if (data_base == NULL)
    {
        fprintf (stderr, "Can't open data base file.\n");
        HashTableDtor (&hash_table);
        return EXIT_FAILURE;
    }
    enum DataBaseError error = ReadDataBase (data_base, &hash_table);
    if (error != kDoneDataBase)
    {
        fprintf (stderr, " Cant create data base.\n Error number = {%d}\n", error);
        fclose (data_base);
        HashTableDtor (&hash_table);
        return EXIT_FAILURE;
    }
    fclose (data_base);
    const size_t kNumIteration = 100'000'000;

    signed long long the_num = 0;
    signed long long Philosophy_num = 0;
    signed long long dragon_num = 0;
    signed long long cock_num = 0;

    // for (size_t iteration = 0; iteration < kNumIteration; iteration++)
    // {
    //     the_num        += HashTableFindElem (hash_table, "the");
    //     Philosophy_num += HashTableFindElem (hash_table, "Philosophy");
    //     dragon_num     += HashTableFindElem (hash_table, "dragon");
    //     cock_num       += HashTableFindElem (hash_table, "cock");
    // }

    // for (size_t iteration = 0; iteration < kNumIteration; iteration++)
    // {
    //     the_num        += HashTableFindElemSIMD (hash_table, "the");
    //     Philosophy_num += HashTableFindElemSIMD (hash_table, "Philosophy");
    //     dragon_num     += HashTableFindElemSIMD (hash_table, "dragon");
    //     cock_num       += HashTableFindElemSIMD (hash_table, "cock");
    // }

    for (size_t iteration = 0; iteration < kNumIteration; iteration++)
    {
        the_num        += ASMHashTableFindElemSIMD (hash_table, "the");
        Philosophy_num += ASMHashTableFindElemSIMD (hash_table, "Philosophy");
        dragon_num     += ASMHashTableFindElemSIMD (hash_table, "dragon");
        cock_num       += ASMHashTableFindElemSIMD (hash_table, "cock");
    }

    fprintf (stdout, "Iteration number = %lu\n"
                        "the:         %lld \n"
                        "Philosophy:  %lld \n"
                        "dragon:      %lld \n"
                        "cock:        %lld \n",
                        kNumIteration,
                        the_num, Philosophy_num, dragon_num, cock_num);

//     HashTablePushElem (hash_table, "slovo");
//     HashTablePushElem (hash_table, "slovo");
//
//     HashTablePushElem (hash_table, "seno");
//     HashTablePushElem (hash_table, "seno");
//     HashTablePushElem (hash_table, "seno");
//
//     HashTablePushElem (hash_table, "den");
//
//     fprintf (stdout, "slovo: %lld \n"
//                      "seno:  %lld \n"
//                      "den:   %lld \n",
//                      HashTableFindElem (hash_table, "slovo"),
//                      HashTableFindElem (hash_table, "seno"),
//                      HashTableFindElem (hash_table, "den"));
//
//     FILE* bucket_size = fopen ("Bucket_size.txt", "w");
//
//     size_t num_words = 0;
//
//     for (size_t bucket_index = 0; bucket_index < kNumBucket; bucket_index++)
//     {
//         fprintf (bucket_size, "bucket_index = %lu bucket size = %lu\n", bucket_index, hash_table [bucket_index].counter);
//         num_words += hash_table [bucket_index].counter;
//     }
//
//     fclose (bucket_size);
//
//     fprintf (stdout, "%lu\n", num_words);

    HashTableDtor (&hash_table);

    fprintf (stdout, "Process time = %lu\n", time (NULL) - start);

    return EXIT_SUCCESS;
}

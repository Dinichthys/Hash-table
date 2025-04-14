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
    const size_t kNumIteration = 1'000'000;
    for (size_t iteration = 0; iteration < kNumIteration; iteration++)
    {
        fprintf (stdout, "Iteration number = %lu\n"
                         "the:         %lld \n"
                         "Philosophy:  %lld \n"
                         "dragon:      %lld \n"
                         "cock:        %lld \n",
                         iteration,
                         HashTableFindElem (hash_table, "the"),
                         HashTableFindElem (hash_table, "Philosophy"),
                         HashTableFindElem (hash_table, "dragon"),
                         HashTableFindElem (hash_table, "cock"));
    }

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

    HashTableDtor (&hash_table);

    fprintf (stdout, "Process time = %lu\n", time (NULL) - start);

    return EXIT_SUCCESS;
}

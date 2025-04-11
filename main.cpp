#include "HashTable/hash_table.h"
#include "DataBase/read_data_base.h"

#include <stdlib.h>
#include <stdio.h>

#include "MyLib/Logger/logging.h"

int main()
{
    set_log_lvl (kDebug);

    hash_table_t hash_table = {};
    HashTableCtor (&hash_table);

    FILE* const data_base = fopen ("DataBase/harrypotter.txt", "r");
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
//
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

    fprintf (stdout, "the: %lld \n"
                     "Philosophy:  %lld \n"
                     "dragon:   %lld \n",
                     HashTableFindElem (hash_table, "the"),
                     HashTableFindElem (hash_table, "Philosophy"),
                     HashTableFindElem (hash_table, "dragon"));

    HashTableDtor (&hash_table);
    return EXIT_SUCCESS;
}

#include "HashTable/hash_table.h"

#include <stdlib.h>
#include <stdio.h>

#include "My_lib/Logger/logging.h"

int main()
{
    set_log_lvl (kDebug);
    hash_table_t hash_table = {};
    HashTableCtor (&hash_table);

    HashTablePushElem (hash_table, "slovo");
    HashTablePushElem (hash_table, "slovo");

    HashTablePushElem (hash_table, "seno");
    HashTablePushElem (hash_table, "seno");
    HashTablePushElem (hash_table, "seno");

    HashTablePushElem (hash_table, "den");

    fprintf (stdout, "slovo: %lld \n"
                     "seno:  %lld \n"
                     "den:   %lld \n",
                     HashTableFindElem (hash_table, "slovo"),
                     HashTableFindElem (hash_table, "seno"),
                     HashTableFindElem (hash_table, "den"));

    HashTableDtor (&hash_table);
    return EXIT_SUCCESS;
}

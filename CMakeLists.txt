cmake_minimum_required (VERSION 3.20)
project (hash-table CXX)

add_executable (${PROJECT_NAME}
    main.cpp
    src/hash.cpp
    src/hash_table_construction.cpp
    src/hash_table_find.cpp
    src/hash_table_push.cpp
    libs/My_lib/Logger/logging.cpp
    libs/My_lib/Assert/print_error.cpp
    libs/List/source/list_construction.cpp
    libs/List/source/list_error_handler.cpp
    libs/List/source/list_src.cpp
    libs/List/source/list_pop.cpp
    libs/List/source/list_push.cpp
)

target_include_directories (${PROJECT_NAME}
    PRIVATE
        libs
        libs/My_lib/Logger
        libs/My_lib/Assert
        libs/List/include
        lins/HashTable
)

target_compile_features (${PROJECT_NAME}
    PRIVATE
        cxx_std_17
)

target_compile_options (${PROJECT_NAME}
    PRIVATE
        -fdiagnostics-color=always

        -Wall
        -Wextra
        -fstack-protector-strong
        -fcheck-new
        -fstrict-overflow

        -march=native
        $<$<CONFIG:Debug>:
            -Og
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -flto
            -DNDEBUG
        >
)

target_link_options (${PROJECT_NAME}
    PRIVATE
        -march=native
        -Wl,-q
        $<$<CONFIG:Debug>:
            -Og
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -flto
        >
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # to generate compile_commands.json

# cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
# cmake --build build

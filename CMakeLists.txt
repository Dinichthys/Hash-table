cmake_minimum_required (VERSION 3.20)
project (hash-table CXX ASM_NASM)

# set(CMAKE_ASM_NASM_LINK_EXECUTABLE "ld <OBJECTS> -o <TARGET>")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

set_source_files_properties(src/HashTable/asm_list_find_elem_simd.asm PROPERTIES LANGUAGE ASM_NASM)

add_executable (${PROJECT_NAME}
    main.cpp
    src/HashTable/hash.cpp
    src/HashTable/hash_table_construction.cpp
    src/HashTable/hash_table_verify.cpp
    src/HashTable/hash_table_find.cpp
    src/HashTable/hash_table_push.cpp

    src/DataBase/read_data_base.cpp

    libs/MyLib/Logger/logging.cpp
    libs/MyLib/Assert/print_error.cpp
    libs/MyLib/My_stdio/my_stdio.cpp

    libs/List/source/list_construction.cpp
    libs/List/source/list_error_handler.cpp
    libs/List/source/list_src.cpp
    libs/List/source/list_pop.cpp
    libs/List/source/list_push.cpp
)

add_library (asm_list_find_elem_simd
    STATIC
        src/HashTable/asm_list_find_elem_simd.asm
)

target_link_libraries (${PROJECT_NAME}
    asm_list_find_elem_simd
)

target_include_directories (${PROJECT_NAME}
    PRIVATE
        libs
        libs/MyLib/My_stdio
        libs/MyLib/Logger
        libs/MyLib/Assert
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

        -g

        $<$<CONFIG:Debug>:
            -Og
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            # -flto
            -DNDEBUG
        >
)

target_link_options (${PROJECT_NAME}
    PRIVATE
        -march=native

        -Wl,-q
        -g

        $<$<CONFIG:Debug>:
            -Og
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            # -flto
            -DNDEBUG
        >
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # to generate compile_commands.json

# cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
# cmake --build build

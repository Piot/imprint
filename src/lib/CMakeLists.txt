cmake_minimum_required(VERSION 3.17.3)
project(imprint C)

set(CMAKE_C_STANDARD 11)


if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(COMPILER_NAME "clang")
    set(COMPILER_CLANG 1)
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_NAME "gcc")
    set(COMPILER_GCC 1)
elseif (CMAKE_C_COMPILER_ID STREQUAL "Intel")
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
endif()


add_compile_definitions(TORNADO_OS_LINUX CONFIGURATION_DEBUG TYRAN_CONFIGURATION_DEBUG)

set(DEPS ../../deps/)

file(GLOB_RECURSE deps_src FOLLOW_SYMLINKS
        "../deps/*/src/*.c"
        )

file(GLOB_RECURSE lib_src FOLLOW_SYMLINKS
        "*.c"
        )

add_library(imprint
        ${lib_src}
        )



if (OS_WINDOWS)
    target_compile_options(imprint PRIVATE /F 5000000 /Wall /wd4668 /wd4820 /wd4100 /wd4710 /wd4711 /wd5945 /wd5105 /wd5045 /we4047 /we4255 /we4013) # /WX
else()
    # -Werror=missing-prototypes
    target_compile_options(imprint PRIVATE -Wall -Wextra -Wshadow -Wstrict-aliasing -ansi -pedantic -Wno-extra-semi-stmt -Wno-undef -Wno-unused-variable -Wno-unused-parameter -Wno-padded -Werror=implicit-function-declaration -Werror=incompatible-pointer-types  -Werror=missing-prototypes -Werror=int-conversion -Werror=return-type) # -Werror=incompatible-pointer-types-discards-qualifiers
    if (COMPILER_CLANG)
        target_compile_options(imprint PRIVATE -Wmost -Weverything -Werror=missing-variable-declarations)
    endif()
endif()


target_include_directories(imprint PUBLIC ${DEPS}piot/clog/src/include)
target_include_directories(imprint PUBLIC ${DEPS}piot/tiny-libc/src/include)
target_include_directories(imprint PUBLIC ../include)

target_link_libraries(imprint -fsanitize=address)

cmake_minimum_required(VERSION 3.17.3)
project(imprint C)

set(CMAKE_C_STANDARD 11)
add_compile_definitions(_POSIX_C_SOURCE=200112L TORNADO_OS_LINUX CONFIGURATION_DEBUG TYRAN_CONFIGURATION_DEBUG)

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

target_include_directories(imprint PUBLIC ${DEPS}piot/clog/src/include)
target_include_directories(imprint PUBLIC ${DEPS}piot/tiny-libc/src/include)
target_include_directories(imprint PUBLIC ../include)

target_link_libraries(imprint -fsanitize=address)

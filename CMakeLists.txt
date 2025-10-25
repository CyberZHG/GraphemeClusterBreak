cmake_minimum_required(VERSION 4.0)
project(GraphemeBreak)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(GRAPHEME_BREAK_ENABLE_STRICT "Use strict compile options" OFF)
option(GRAPHEME_BREAK_ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(GRAPHEME_BREAK_ENABLE_TESTS "Build tests" OFF)

if(APPLE)
    set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

if(GRAPHEME_BREAK_ENABLE_STRICT)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        add_compile_options(
                -Wall
                -Wextra
                -Werror
                -Wpedantic
                -Wmissing-include-dirs
                -Wundef
                -Wredundant-decls
        )
    endif()
endif()

if(GRAPHEME_BREAK_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g -fno-elide-constructors -fno-default-inline)
        add_link_options(--coverage)
    endif()
endif()

add_library(GraphemeBreak STATIC
        include/break_properties.h
        src/break_properties.cpp
        include/grapheme_cluster.h
        src/grapheme_cluster.cpp
)

target_include_directories(GraphemeBreak
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(GRAPHEME_BREAK_ENABLE_TESTS)
    enable_testing()

    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(runTests
            tests/main.cpp
            tests/test_break_properties.cpp
            tests/test_grapheme_break_all.cpp
            tests/test_grapheme_break_basic.cpp
    )

    target_link_libraries(runTests
            PRIVATE GraphemeBreak
            PRIVATE gtest gtest_main
    )

    add_test(NAME GraphemeBreakTests COMMAND runTests)

endif()
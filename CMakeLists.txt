cmake_minimum_required(VERSION 4.0)
project(GraphemeClusterBreak)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(GRAPHEME_CLUSTER_BREAK_ENABLE_STRICT "Use strict compile options" OFF)
option(GRAPHEME_CLUSTER_BREAK_ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(GRAPHEME_CLUSTER_BREAK_ENABLE_TESTS "Build tests" OFF)
option(GRAPHEME_CLUSTER_BREAK_ENABLE_WORD_BREAK "Enable word break segmentation" OFF)
option(GRAPHEME_CLUSTER_BREAK_ENABLE_SENTENCE_BREAK "Enable sentence break segmentation" OFF)
option(GRAPHEME_CLUSTER_BREAK_BIND_PYTHON "Enable PYTHON binding" OFF)
option(GRAPHEME_CLUSTER_BREAK_BIND_ES "Enable ECMAScript binding" OFF)

if(APPLE)
    set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

if(GRAPHEME_CLUSTER_BREAK_ENABLE_STRICT)
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

if(GRAPHEME_CLUSTER_BREAK_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g -fno-elide-constructors -fno-default-inline)
        add_link_options(--coverage)
    endif()
endif()

set(GRAPHEME_CLUSTER_BREAK_SOURCES
        include/unicode_utils.h
        src/unicode_utils.cpp
        include/emoji_pictographic_properties.h
        src/emoji_pictographic_properties.cpp
        include/grapheme_break_properties.h
        src/grapheme_break_properties.cpp
        include/grapheme_break.h
        src/grapheme_break.cpp
)

if(GRAPHEME_CLUSTER_BREAK_ENABLE_WORD_BREAK)
    list(APPEND GRAPHEME_CLUSTER_BREAK_SOURCES
            include/word_break_properties.h
            src/word_break_properties.cpp
            include/word_break.h
            src/word_break.cpp
    )
endif()

if(GRAPHEME_CLUSTER_BREAK_ENABLE_SENTENCE_BREAK)
    list(APPEND GRAPHEME_CLUSTER_BREAK_SOURCES
            include/sentence_break_properties.h
            src/sentence_break_properties.cpp
    )
endif()

add_library(GraphemeClusterBreak STATIC ${GRAPHEME_CLUSTER_BREAK_SOURCES})

target_include_directories(GraphemeClusterBreak
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(GRAPHEME_CLUSTER_BREAK_ENABLE_WORD_BREAK)
    target_compile_definitions(GraphemeClusterBreak PUBLIC GRAPHEME_CLUSTER_BREAK_WORD_BREAK_ENABLED)
endif()

if(GRAPHEME_CLUSTER_BREAK_ENABLE_TESTS)
    enable_testing()

    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
    )

    FetchContent_MakeAvailable(googletest)

    set(TEST_SOURCES
            tests/main.cpp
            tests/test_emoji_properties.cpp
            tests/test_grapheme_break_properties.cpp
            tests/test_grapheme_break_basic.cpp
            tests/test_grapheme_break_all.cpp
            tests/test_grapheme_break_all_utf8.cpp
    )

    if(GRAPHEME_CLUSTER_BREAK_ENABLE_WORD_BREAK)
        list(APPEND TEST_SOURCES
                tests/test_word_break_properties.cpp
                tests/test_word_break_basic.cpp
                tests/test_word_break_all.cpp
                tests/test_word_break_all_utf8.cpp
        )
    endif()

    if(GRAPHEME_CLUSTER_BREAK_ENABLE_SENTENCE_BREAK)
        list(APPEND TEST_SOURCES
                tests/test_sentence_break_properties.cpp
        )
    endif()

    add_executable(runTests ${TEST_SOURCES})

    target_link_libraries(runTests
            PRIVATE GraphemeClusterBreak
            PRIVATE gtest gtest_main
    )

    add_test(NAME GraphemeClusterBreakTests COMMAND runTests)

endif()

if(GRAPHEME_CLUSTER_BREAK_BIND_PYTHON)
    include(FetchContent)
    FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11.git
            GIT_TAG v3.0
    )
    FetchContent_MakeAvailable(pybind11)

    pybind11_add_module(_core
            python/binding.cpp
    )
    target_link_libraries(_core
            PRIVATE GraphemeClusterBreak
    )
    install(TARGETS _core
            LIBRARY DESTINATION grapheme_cluster_break)
    install(FILES python/wrapper/__init__.py DESTINATION grapheme_cluster_break)
endif()

if (GRAPHEME_CLUSTER_BREAK_BIND_ES)
    add_executable(GraphemeClusterBreakWASM
            wasm/binding.cpp)

    target_link_libraries(GraphemeClusterBreakWASM
            PRIVATE GraphemeClusterBreak
    )

    target_compile_options(GraphemeClusterBreakWASM PRIVATE "-O3")

    set_target_properties(GraphemeClusterBreakWASM PROPERTIES SUFFIX ".js")

    target_link_options(GraphemeClusterBreakWASM PRIVATE
            "--bind"
            "-sMODULARIZE=1"
            "-sEXPORT_ES6=1"
            "-sEXPORT_NAME=GraphemeClusterBreakWASMModule"
            "-sNO_DISABLE_EXCEPTION_CATCHING"
    )
endif ()

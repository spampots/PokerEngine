cmake_minimum_required(VERSION 3.20)

project(
    poker_engine
    VERSION 1.0
    DESCRIPTION "C++ Poker Engine with Kuhn Poker and Counterfactual Regret Minimization"
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# C++ configuration
# ---------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Compiler warnings
# ---------------------------------------------------------------------------

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------

add_library(
    poker_engine
    STATIC

    src/Card.cpp
    src/GameState.cpp
    src/InformationSet.cpp
    src/KuhnPoker.cpp
    src/CFR.cpp
)

target_include_directories(
    poker_engine
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# ---------------------------------------------------------------------------
# Example executable
# ---------------------------------------------------------------------------

add_executable(
    train_kuhn
    examples/train_kuhn.cpp
)

target_link_libraries(
    train_kuhn
    PRIVATE
        poker_engine
)

# ---------------------------------------------------------------------------
# Testing
# ---------------------------------------------------------------------------

include(CTest)

if(BUILD_TESTING)

    # Fetch GoogleTest automatically.
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )

    # Prevent GoogleTest from overriding the project's runtime library
    # configuration on MSVC.
    set(
        gtest_force_shared_crt
        ON
        CACHE BOOL ""
        FORCE
    )

    FetchContent_MakeAvailable(googletest)

    enable_testing()

    # -----------------------------------------------------------------------
    # Test executable
    # -----------------------------------------------------------------------

    add_executable(
        poker_tests

        tests/test_cards.cpp
        tests/test_game_state.cpp
        tests/test_kuhn.cpp
        tests/test_cfr.cpp
        tests/test_end_to_end.cpp
    )

    target_link_libraries(
        poker_tests
        PRIVATE
            poker_engine
            GTest::gtest_main
    )

    # Automatically discover TEST() cases.
    include(GoogleTest)

    gtest_discover_tests(
        poker_tests
    )

endif()
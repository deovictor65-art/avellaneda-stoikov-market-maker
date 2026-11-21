cmake_minimum_required(VERSION 3.20)
project(avellaneda_stoikov_market_maker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4 /O2 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O3 -march=native)
endif()

add_library(market_maker_core INTERFACE)
target_include_directories(market_maker_core INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_executable(market_maker_sim src/main.cpp)
target_link_libraries(market_maker_sim PRIVATE market_maker_core)

enable_testing()
add_executable(market_maker_tests tests/test_market_maker.cpp)
target_link_libraries(market_maker_tests PRIVATE market_maker_core)

add_test(NAME MarketMakerTests COMMAND market_maker_tests)

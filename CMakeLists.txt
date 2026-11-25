cmake_minimum_required(VERSION 3.25)        # using Version 3.25
project(kestrel LANGUAGES CXX)              # project name

set(CMAKE_CXX_STANDARD 20)               
set(CMAKE_CXX_STANDARD_REQUIRED ON)         # fail instead of silently using older C++
set(CMAKE_CXX_EXTENSIONS OFF)               # -std=c++20, not -std=gnu++20
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)       # writes compile_commands.json for clangd

add_library(kestrel_warnings INTERFACE)
target_compile_options(kestrel_warnings INTERFACE -Wall -Wextra -Werror)

# A header-only library: also INTERFACE, plus where its headers live:
add_library(legacy_book INTERFACE)
target_include_directories(legacy_book INTERFACE src/legacy_book/include)

add_executable(legacy_main_test src/legacy_book/src/main.cpp)
target_link_libraries(legacy_main_test PRIVATE legacy_book kestrel_warnings)

add_executable(price_level_test tests/legacy_book/price_level_test.cpp)
target_link_libraries(price_level_test PRIVATE legacy_book kestrel_warnings)

enable_testing()                                             # turns on CTest
add_test(NAME legacy_main_test COMMAND legacy_main_test)     # "run this program; exit code 0 = pass"
add_test(NAME price_level_test COMMAND price_level_test)     # "run this program; exit code 0 = pass"
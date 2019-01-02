cmake_minimum_required(VERSION 3.1)
project (libapdu)

set(CMAKE_CXX_STANDARD 11)

add_library(apdu STATIC src/apdu.cpp)

add_executable(unit_tests src/test.cpp)
target_link_libraries(unit_tests apdu)

enable_testing()
add_test(NAME unit-tests COMMAND unit_tests)

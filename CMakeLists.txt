cmake_minimum_required(VERSION 3.3)
project (libapdu)
add_library(apdu STATIC src/apdu.cpp)

add_executable(unit_tests src/test.cpp)
target_link_libraries(unit_tests apdu)

enable_testing()
add_test(NAME unit-tests COMMAND unit_tests)

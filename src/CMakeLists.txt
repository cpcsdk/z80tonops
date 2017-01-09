cmake_minimum_required(VERSION 3.5)

project (z80tonops CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -DNDEBUG")


add_library(libz80tonops z802nops.cpp z802nops.h utils.h)
add_executable(z80tonops main.cpp)
add_executable(t test.cpp)


#target_compile_features(libz80tonops PRIVATE cxx_regex) # regex feature does not exists ...
target_compile_features(libz80tonops PRIVATE cxx_digit_separators) # ensure we are using c++>=11
target_compile_features(z80tonops PRIVATE cxx_digit_separators) # ensure we are using c++>=11
target_compile_features(t PRIVATE cxx_digit_separators) # ensure we are using c++>=11

target_link_libraries(z80tonops libz80tonops)
target_link_libraries(t libz80tonops)

install(TARGETS z80tonops libz80tonops
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)


# basic project setup
cmake_minimum_required(VERSION 3.1...3.24)
project(
  nsStep9cmp
  VERSION 1.0
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)

# define library for shared utility code
add_library(ns_utils utils.cpp utils.h)

add_executable(nsStep89 nsStep8-9.cpp nsStep8-9.h)
add_executable(nsStep7 nsStep7.cpp nsStep7.h)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(nsStep89 PUBLIC OpenMP::OpenMP_CXX ns_utils)
    target_link_libraries(nsStep7 PUBLIC OpenMP::OpenMP_CXX ns_utils)
endif()

if($ENV{BUILDFORTEST})
    add_compile_definitions(CREATEPLOTS)
    add_compile_definitions(TERMONTOL)
endif()

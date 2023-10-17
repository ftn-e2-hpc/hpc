cmake_minimum_required(VERSION 3.5)
project(GeneticAlgorithm)

find_package(OpenMP)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
set(SOURCE_FILES main.c)

add_executable(GeneticAlgorithm ${SOURCE_FILES})

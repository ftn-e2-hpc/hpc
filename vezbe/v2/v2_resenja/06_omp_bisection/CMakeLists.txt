cmake_minimum_required(VERSION 3.5)
project(BisectionMethod)

find_package(OpenMP)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} -lm")
set(SOURCE_FILES main.c)

add_executable(BisectionMethod ${SOURCE_FILES})
target_link_libraries(BisectionMethod -lm)

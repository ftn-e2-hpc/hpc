cmake_minimum_required(VERSION 3.5)
project(MatrixMultiplication)

find_package(OpenMP REQUIRED)
find_package(HDF5)

# additional options
option(ENABLE_HDF5 "Enable HDF5 support." ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
set(SOURCE_FILES main.c)

add_executable(MatrixMultiplication ${SOURCE_FILES})

if(ENABLE_HDF5)
    if(HDF5_FOUND)
        # added to remove the warning caused by newer cmake versions that expand relative paths to absolute paths
        # can be done with set_policy(CMP0075) with cmake versions >= 3.13
        get_filename_component(H5DEFS ./utils/h5defs.h ABSOLUTE)
        get_filename_component(H5MATUTILS_C ./utils/h5_matrix_utils.c ABSOLUTE)
        get_filename_component(H5MATUTILS_H ./utils/h5_matrix_utils.h ABSOLUTE)

        include_directories(${HDF5_INCLUDE_DIR})
        target_link_libraries(MatrixMultiplication ${HDF5_C_LIBRARIES})
        set(HDF5_SOURCE_FILES ${H5DEFS} ${H5MATUTILS_C} ${H5MATUTILS_H})
        target_sources(MatrixMultiplication PUBLIC ${HDF5_SOURCE_FILES})
    else()
        message(FATAL_ERROR "HDF5 support has been requested, but no HDF5 library was found!")
    endif()
else()
    add_definitions(-DDISABLE_HDF5)
    message(STATUS "HDF5 support disabled.")
endif()

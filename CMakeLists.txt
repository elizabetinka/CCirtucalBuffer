cmake_minimum_required(VERSION 3.12)

project(
    labwork8
    VERSION 1.0
)

set(CMAKE_CXX_STANDARD 17)


add_subdirectory(lib)
add_subdirectory(bin)


enable_testing()
add_subdirectory(tests)

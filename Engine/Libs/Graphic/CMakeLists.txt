cmake_minimum_required(VERSION 3.16)
project(RteGraphic CXX)

# Options
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS FALSE)    # Build static libraries

# Submodule libraries
add_subdirectory(${CMAKE_SOURCE_DIR}/external/raylib ${CMAKE_BINARY_DIR}/raylib)

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Link libraries and target
add_library(RteGraphic STATIC ${SOURCES})
target_link_libraries(RteGraphic PRIVATE raylib)

# Include directories
target_include_directories(RteGraphic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

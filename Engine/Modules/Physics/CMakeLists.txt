cmake_minimum_required(VERSION 3.16)
project(RtePhysics CXX)

# Options
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS FALSE)    # Build static libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Submodule libraries
add_subdirectory(${CMAKE_SOURCE_DIR}/external/box2d ${CMAKE_BINARY_DIR}/box2d)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/poly2tri ${CMAKE_BINARY_DIR}/poly2tri)

# Debug flags if gcc
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")   # TODO: remove this
endif()

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Link libraries and target
add_library(RtePhysics SHARED ${SOURCES})
target_link_libraries(RtePhysics PRIVATE RteCore box2d poly2tri)

# Include directories
target_include_directories(RtePhysics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

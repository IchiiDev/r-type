cmake_minimum_required(VERSION 3.20)
project(Engine CXX)

# Options
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS FALSE)    # Build static libraries

# Submodule libraries
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SFML ${CMAKE_BINARY_DIR}/SFML)

# Debug flags if gcc
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")   # TODO: remove this
endif()

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Link libraries and target
add_library(Engine STATIC ${SOURCES})
target_link_libraries(Engine PRIVATE sfml-graphics sfml-window sfml-system)

# Include directories
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

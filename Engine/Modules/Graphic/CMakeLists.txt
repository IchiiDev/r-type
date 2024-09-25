cmake_minimum_required(VERSION 3.16)
project(RteGraphic CXX)

# Options
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS FALSE)    # Build static libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Submodule libraries
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SFML ${CMAKE_BINARY_DIR}/SFML)

# Debug flags if gcc
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fsanitize=address")   # TODO: remove this
endif()

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Link libraries and target
add_library(RteGraphic SHARED ${SOURCES})
target_link_libraries(RteGraphic PRIVATE RteCore sfml-graphics sfml-window sfml-system)

# Include directories
target_include_directories(RteGraphic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

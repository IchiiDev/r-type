cmake_minimum_required(VERSION 3.16)
project(RteAudio CXX)

# Options
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS FALSE)    # Build static libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Debug flags if gcc
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")   # TODO: remove this
endif()

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Link libraries and target
add_library(RteAudio SHARED ${SOURCES})
target_link_libraries(RteAudio PRIVATE RteCore sfml-audio)

# Include directories
target_include_directories(RteAudio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

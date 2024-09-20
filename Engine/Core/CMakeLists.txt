cmake_minimum_required(VERSION 3.16)
project(RteCore CXX)

# Options
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS FALSE)    # Build static libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# External dependencies
add_subdirectory(${CMAKE_SOURCE_DIR}/external/spdlog ${CMAKE_BINARY_DIR}/spdlog)

# Debug flags if gcc
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")   # TODO: remove this
endif()

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Link libraries and target
add_library(RteCore STATIC ${SOURCES})
target_link_libraries(RteCore PRIVATE spdlog)

# Include directories
target_include_directories(RteCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

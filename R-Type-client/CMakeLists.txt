cmake_minimum_required(VERSION 3.16)
project(R-Type-client CXX)

# Options
set(CMAKE_CXX_STANDARD 20)

# Debug flags if gcc
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fsanitize=address")   # TODO: remove this
endif()

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Link libraries and target
add_executable(R-Type-client ${SOURCES})
target_link_libraries(R-Type-client PRIVATE RteCore)

# Include directories
target_include_directories(R-Type-client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

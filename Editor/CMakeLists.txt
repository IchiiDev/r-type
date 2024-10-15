cmake_minimum_required(VERSION 3.16)
project(RteEditor CXX)

# Options
set(CMAKE_CXX_STANDARD 20)

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Link libraries and target
add_executable(RteEditor ${SOURCES})
target_link_libraries(RteEditor PRIVATE RteGraphic)

# Include directories
target_include_directories(RteEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

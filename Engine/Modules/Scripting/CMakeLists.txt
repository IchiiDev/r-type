cmake_minimum_required(VERSION 3.16)
project(RteScripting CXX)

# Options
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS FALSE)    # Build static libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Lua build
file(GLOB lua_sources ${CMAKE_SOURCE_DIR}/external/lua/*.c)
list(REMOVE_ITEM lua_sources "${CMAKE_SOURCE_DIR}/external/lua/lua.c" "${CMAKE_SOURCE_DIR}/external/lua/luac.c")
add_library(lua STATIC ${lua_sources})

target_include_directories(lua
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/lua>
)

# Luacpp submodule
add_subdirectory(${CMAKE_SOURCE_DIR}/external/sol2 ${CMAKE_BINARY_DIR}/external/sol2)

# Debug flags if gcc
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")   # TODO: remove this
endif()

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Link libraries and target
add_library(RteScripting SHARED ${SOURCES})
target_link_libraries(RteScripting PRIVATE RteCore lua sol2)

# Include directories
target_include_directories(RteScripting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

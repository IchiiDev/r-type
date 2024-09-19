cmake_minimum_required(VERSION 3.16)
project(Build-All CXX)

# Include directories of modules
include_directories(${CMAKE_SOURCE_DIR}/Engine/Modules/Graphic/include)

# Game engine
add_subdirectory(Engine)

# Game client
add_subdirectory(R-Type-client)

# Game server
add_subdirectory(R-Type-server)

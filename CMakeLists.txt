cmake_minimum_required(VERSION 3.16)
project(Build-All CXX)

# Include directories of modules
include_directories(${CMAKE_SOURCE_DIR}/Engine/Modules/Graphic/include)
include_directories(${CMAKE_SOURCE_DIR}/Engine/Modules/Network/include)
include_directories(${CMAKE_SOURCE_DIR}/Engine/Modules/Physics/include)
include_directories(${CMAKE_SOURCE_DIR}/Engine/Modules/Audio/include)
include_directories(${CMAKE_SOURCE_DIR}/Engine/Modules/Scripting/include)

# Game engine
add_subdirectory(Engine)

# Game client
add_subdirectory(R-Type-client)

# Game server
add_subdirectory(R-Type-server)

# Game demo
add_subdirectory(R-Type-demo)

# Flappy bird
add_subdirectory(FlappyBird)

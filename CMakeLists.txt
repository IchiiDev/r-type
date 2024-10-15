cmake_minimum_required(VERSION 3.16)
project(RteBuildAll CXX)

# Engine
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine)

# Editor
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Editor)

# Apps
# ...

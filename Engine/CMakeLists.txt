cmake_minimum_required(VERSION 3.16)
project(Engine CXX)

# Modules
add_subdirectory(Modules/Graphic)
add_subdirectory(Modules/Network)

# Core
add_subdirectory(Core)

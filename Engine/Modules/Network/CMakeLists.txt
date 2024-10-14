project(RteNetwork CXX)

# Options
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS FALSE)    # Build static libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include directories for ASIO (since it's a header-only library)
set(ASIO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/asio/asio/include)

# Debug flags if using GCC
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3")   # TODO: remove this in production
endif()

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Link libraries and target
add_library(RteNetwork SHARED ${SOURCES})
target_link_libraries(RteNetwork PRIVATE RteCore)

# Include directories
target_include_directories(RteNetwork PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(RteNetwork PRIVATE ${ASIO_INCLUDE_DIR})

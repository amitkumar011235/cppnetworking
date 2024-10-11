cmake_minimum_required(VERSION 3.20)

# Set the project name and version
project(tcpdemo VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Detect the platform and define a variable accordingly
if (WIN32)
    set(PLATFORM_NAME "windows")
    add_definitions(-DPLATFORM_WINDOWS)
elseif (UNIX AND NOT APPLE)
    set(PLATFORM_NAME "linux")
    add_definitions(-DPLATFORM_LINUX)
elseif (APPLE)
    set(PLATFORM_NAME "mac")
    add_definitions(-DPLATFORM_MAC)
endif()

# Options to control which components to build
# Option to control what to build: can be "all", "tcpclient", "tcpserver", or "example"
set(BUILD_TARGET "all" CACHE STRING "Choose which target to build (all, tcpclient, tcpserver, example)")

# Set output directory for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug)


# Add subdirectories conditionally based on the BUILD_TARGET option
if (BUILD_TARGET STREQUAL "all" OR BUILD_TARGET STREQUAL "tcpserver")
    add_subdirectory(tcpserver)
endif()

if (BUILD_TARGET STREQUAL "all" OR BUILD_TARGET STREQUAL "tcpclient")
    add_subdirectory(tcpclient)
endif()

if (BUILD_TARGET STREQUAL "all" OR BUILD_TARGET STREQUAL "example")
    add_subdirectory(example)
endif()

# Add subdirectories conditionally based on the BUILD_TARGET option
if (BUILD_TARGET STREQUAL "all" OR BUILD_TARGET STREQUAL "httpserver")
    add_subdirectory(httpserver)
endif()

# print the platform for debug purposes
message(STATUS "Building for platform: ${PLATFORM_NAME}")
message(STATUS "Build target: ${BUILD_TARGET}")

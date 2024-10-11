# Specify the project name
project(httpserver)

# Include OS-specific implementation based on the platform
if(WIN32)
    set(SERVER_OS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../tcpserver/win/win.cpp")
elseif(UNIX AND NOT APPLE)
    set(SERVER_OS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../tcpserver/lin/lin.cpp")
elseif(APPLE)
    set(SERVER_OS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../tcpserver/mac/mac.cpp")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Find all .cpp files in the httpserver folder
file(GLOB HTTP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Add the OS-specific and httpserver source files to the library
add_library(httpserver STATIC ${SERVER_OS_SRC} ${HTTP_SOURCES})

target_link_libraries(httpserver tcpserver)

# Specify include directories for the target
target_include_directories(httpserver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

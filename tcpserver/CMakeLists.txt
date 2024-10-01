project(tcpserver)

# Include the OS-specific implementation
if(WIN32)
    set(SERVER_OS_SRC win/win.cpp)
elseif(UNIX AND NOT APPLE)
    set(SERVER_OS_SRC lin/lin.cpp)
elseif(APPLE)
    set(SERVER_OS_SRC mac/mac.cpp)
endif()

# Add tcpserver.cpp
add_library(tcpserver STATIC tcpserver.cpp ${SERVER_OS_SRC})

target_include_directories(tcpserver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

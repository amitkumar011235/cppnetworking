project(tcpclient)

# Include the OS-specific implementation
if(WIN32)
    set(CLIENT_OS_SRC win/win.cpp)
elseif(UNIX AND NOT APPLE)
    set(CLIENT_OS_SRC lin/lin.cpp)
elseif(APPLE)
    set(CLIENT_OS_SRC mac/mac.cpp)
endif()

# Add tcpserver.cpp
add_library(tcpclient STATIC tcpclient.cpp ${CLIENT_OS_SRC})

target_include_directories(tcpclient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_link_libraries(tcpclient ws2_32)
endif()

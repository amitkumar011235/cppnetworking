project(tcpserver)

# Include the OS-specific implementation
if(WIN32)
    set(SERVER_OS_SRC win/win.cpp)
elseif(UNIX AND NOT APPLE)
    set(SERVER_OS_SRC lin/lin.cpp)
elseif(APPLE)
    set(SERVER_OS_SRC mac/mac.cpp)
endif()


# Find the OpenSSL library.
find_package(OpenSSL REQUIRED)

# Add tcpserver.cpp
add_library(tcpserver STATIC tcpserver.cpp ${SERVER_OS_SRC})

# Link the OpenSSL libraries to the target.
target_link_libraries(tcpserver PUBLIC OpenSSL::SSL OpenSSL::Crypto)

target_include_directories(tcpserver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

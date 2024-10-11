project(demo)

set(CMAKE_CXX_STANDARD 17)


#server example build
add_executable(server server.cpp)
target_link_libraries(server tcpserver)
set(INCLUDE_SERVER_DIRS "../tcpserver")
target_include_directories(server PUBLIC ${INCLUDE_SERVER_DIRS})



#client example build
add_executable(client client.cpp)
target_link_libraries(client tcpclient)
set(INCLUDE_CLIENT_DIRS "../tcpclient")
target_include_directories(client PUBLIC ${INCLUDE_CLIENT_DIRS})


#http server build
#server example build
add_executable(httpserverdemo httpserver.cpp)
target_link_libraries(httpserverdemo httpserver)
set(INCLUDE_SERVER_DIRS "../httpserver")
target_include_directories(server PUBLIC ${INCLUDE_SERVER_DIRS})


# Link the example executable with the tcpserver static library











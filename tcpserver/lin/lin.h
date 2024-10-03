#ifndef LINSERVER_H
#define LINSERVER_H

#include "../tcpserver.h"
#include <string>
#include <sys/epoll.h>
#include "ctpl_stl.h"

class LinServer : public TCPServer
{
public:
    LinServer();
    virtual ~LinServer();

    bool initialize(int port, const std::string &ip_address = "0.0.0.0") override;
    void start() override;
    void handleClient(int client_socket);
    bool setSocketNonBlocking(int socketId);              


private:
    int server_fd;                        // File descriptor for the server socket
    int epoll_fd;                         // File descriptor for epoll
    std::thread epollThread;              // dedicated thread for epoll_wait()
    static ctpl::thread_pool threadPool;  // Static thread pool shared by all instances
    static int determineThreadPoolSize(); // Helper to determine the thread pool size
    void handleRecv(int client_socket);
    void handleSend(int client_socket);
    void handleError(int client_socket);
    void epollLoop(); // The dedicated epoll thread
};

#endif // LINSERVER_H

#ifndef LINSERVER_H
#define LINSERVER_H

#include "../tcpserver.h"
#include <string>
#include "ctpl_stl.h"

class LinServer : public TCPServer
{
public:
    LinServer();
    virtual ~LinServer();

    bool initialize(int port, const std::string &ip_address = "0.0.0.0") override;
    void start() override;
    void handleClient(int client_socket);

private:
    int server_fd;                        // File descriptor for the server socket
    static ctpl::thread_pool threadPool;  // Static thread pool shared by all instances
    static int determineThreadPoolSize(); // Helper to determine the thread pool size
};

#endif // LINSERVER_H

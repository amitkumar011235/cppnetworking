#ifndef WIN_H
#define WIN_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include "../tcpserver.h"
#include <string>
#include "ctpl_stl.h"

class WinServer : public TCPServer
{
private:
    int port;
    std::string ipAddress;
    SOCKET server_fd;
    static ctpl::thread_pool threadPool;  // Static thread pool shared by all instances
    static int determineThreadPoolSize(); // Helper to determine the thread pool size

public:
    bool initialize(int port, const std::string &ipAddress = "0.0.0.0") override;
    void start() override;
    void handleClient(SOCKET clientSocket);
    bool setSocketNonBlocking(SOCKET socket_fd);
    ~WinServer();
};

#endif // WIN_H

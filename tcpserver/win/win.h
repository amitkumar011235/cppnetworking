#ifndef WIN_H
#define WIN_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include "../tcpserver.h"
#include <string>
#include "ctpl_stl.h"

#define BUFFER_SIZE 1024

enum class OperationType { RECV, SEND };

class WinServer : public TCPServer
{
private:
    int port;
    std::string ipAddress;
    SOCKET server_fd;
    HANDLE iocpHandle;

    struct PerIoData
    {
        OVERLAPPED overlapped;
        WSABUF wsabuf;
        char buffer[BUFFER_SIZE];
        DWORD bytesSent;
        DWORD bytesRecv;
        SOCKET clientSocket;
        OperationType operation; // To indicate whether it's RECV or SEND
    };

    static ctpl::thread_pool threadPool;  // Static thread pool shared by all instances
    static int determineThreadPoolSize(); // Helper to determine the thread pool size
    void handleRecv(PerIoData* ioData);
    void handleSend(PerIoData* ioData);
    void handleError(PerIoData* ioData);
    void workerThread(); // IOCP Worker thread

public:
    WinServer();
    bool initialize(int port, const std::string &ipAddress = "0.0.0.0") override;
    void start() override;
    void handleClient(SOCKET clientSocket);
    bool setSocketNonBlocking(SOCKET socket_fd);
    ~WinServer();
};

#endif // WIN_H

#ifndef WIN_H
#define WIN_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include "../tcpserver.h"
#include <string>

class WinServer : public TCPServer {
private:
    int port;
    std::string ipAddress;
    SOCKET server_fd;

public:
    bool initialize(int port, const std::string& ipAddress = "0.0.0.0") override;
    void start() override;
    void handleClient(SOCKET clientSocket);
    ~WinServer();
};

#endif // WIN_H

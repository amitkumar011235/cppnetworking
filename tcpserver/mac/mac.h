#ifndef MACSERVER_H
#define MACSERVER_H

#include "../tcpserver.h"
#include <string>

class MacServer : public TCPServer {
public:
    MacServer();
    virtual ~MacServer();

    bool initialize(int port, const std::string& ip_address = "0.0.0.0") override;
    void start() override;

private:
    int server_fd;  // File descriptor for the server socket
    int new_socket; // File descriptor for the client connection
};

#endif // MACSERVER_H

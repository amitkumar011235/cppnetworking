#ifndef LINSERVER_H
#define LINSERVER_H

#include "../tcpserver.h"
#include <string>

class LinServer : public TCPServer {
public:
    LinServer();
    virtual ~LinServer();
    
    bool initialize(int port, const std::string& ip_address = "0.0.0.0") override;
    void start() override;
    void handleClient(int client_socket) ;

private:
    int server_fd;  // File descriptor for the server socket
};

#endif // LINSERVER_H

#ifndef MAC_H
#define MAC_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cerrno>

#include "../tcpclient.h"

class MacClient : public TCPClient {
private:
    int clientSocket;

public:
    MacClient();
    bool initialize();                                      // Initialize macOS socket
    bool start(const std::string &ip, unsigned short port); // Connect to server
    bool sendData(const std::string &data);                 // Send data to server
    std::string receiveData(size_t maxLength = 1024);       // Receive data from server
    ~MacClient();
};

#endif // MAC_H

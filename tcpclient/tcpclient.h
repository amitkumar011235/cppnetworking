#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
using namespace std;

class TCPClient {
public:
    virtual bool initialize() = 0;                                      // Initialize Winsock
    virtual bool start(const std::string &ip, unsigned short port) = 0; // Connect to server
    virtual bool sendData(const std::string &data) = 0;                 // Send data to server
    virtual string receiveData(size_t maxLength = 1024) = 0;       // Receive data from server
};

// Factory function to create the appropriate server instance based on the OS
TCPClient* createClient();

#endif // TCPCLIENT_H

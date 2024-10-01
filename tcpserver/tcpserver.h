#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>

class TCPServer {
public:
    virtual bool initialize(int port, const std::string& ipAddress = "127.0.0.1") = 0; // Initialize with port and IP
    virtual void start() = 0;  // Start the server and listen for connections
    virtual ~TCPServer() = default;
};

// Factory function to create the appropriate server instance based on the OS
TCPServer* createServer();

#endif // TCPSERVER_H

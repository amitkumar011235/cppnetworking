#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
using namespace std;

class TCPClient {
public:
    virtual bool initialize() = 0;                                      // Initialize Winsock
    virtual bool start(const std::string &ip, unsigned short port) = 0; // Connect to server
    virtual bool sendData(const std::string &data, int length = -1, const std::string &delimiter = "\r\n\r\n") = 0;  
    virtual string receiveData(size_t maxLength = 1024) = 0;       // Receive data from server
    virtual string recvData() = 0;                                  // Receive data from server
    virtual void setTimeout(int seconds) = 0;                           // New API to set the timeout 
    virtual void getRecvSocketBuffer() = 0;    
    virtual void getSendSocketBuffer() = 0;
    virtual void setRecvSocketBuffer(int recv_size) = 0; 
    virtual void setSendSocketBuffer(int send_size) = 0;  
    virtual bool setSocketNonBlocking() = 0;
};

// Factory function to create the appropriate server instance based on the OS
TCPClient* createClient();

#endif // TCPCLIENT_H

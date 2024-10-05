#ifndef LIN_H
#define LIN_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cerrno>

#include "../tcpclient.h"

class LinClient : public TCPClient {
private:
    int clientSocket;
    bool sentDelimiterHeader; // Tracks if the delimiter header has been sent
    struct timeval timeout; // Timeout value for the socket
    std::string receiveHeader();
    std::string receiveFixedLengthContent(int contentLength);
    std::string receiveUntilDelimiter(const std::string &delimiter);
    bool sendAll(const char *buffer, size_t length);

public:
    LinClient();
    bool initialize();                                      // Initialize Linux socket
    bool start(const std::string &ip, unsigned short port); // Connect to server
    bool sendDataold(const std::string &data);                 // Send data to server
    std::string receiveData(size_t maxLength = 1024);       // Receive data from server
    void setTimeout(int seconds);                           // New API to set the timeout 
    void getRecvSocketBuffer();    
    void getSendSocketBuffer();
    void setRecvSocketBuffer(int recv_size); 
    void setSendSocketBuffer(int send_size);   
    bool setSocketNonBlocking(); 
    std::string recvData();   
    bool sendData(const std::string &data, int length = -1, const std::string &delimiter = "\r\n\r\n");          
    ~LinClient();
};

#endif // LIN_H

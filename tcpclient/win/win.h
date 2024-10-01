#ifndef WIN_H
#define WIN_H

#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef byte
#undef byte
#endif

#include "../tcpclient.h"
#include <string>



class WinClient : public TCPClient {
private:
    WSADATA wsaData;
    SOCKET clientSocket;

public:
    WinClient();
    bool initialize();                                      // Initialize Winsock
    bool start(const std::string &ip, unsigned short port); // Connect to server
    bool sendData(const std::string &data);                 // Send data to server
    std::string receiveData(size_t maxLength = 1024);       // Receive data from server
    ~WinClient();
};

#endif // WIN_H

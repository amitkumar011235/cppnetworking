#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>

#ifdef PLATFORM_WINDOWS
#include <winsock2.h>
#endif

using namespace std;

struct ClientState
{
#ifdef PLATFORM_WINDOWS
    SOCKET client_socket;
#elif defined(PLATFORM_LINUX)
    int client_socket;
#elif defined(PLATFORM_MAC)
    int client_socket;
#endif

    SSL* ssl;                              //storing ssl wrapper for the client_socket
    string recvBuffer;                     // Buffer for holding incoming data
    string sendBuffer;                     // Buffer for holding outgoing data
    size_t bytesSent;                      // Tracks how many bytes have been sent
    bool waitingForSend;                   // True if waiting to send more data
    bool waitingForRecv;                   // True if waiting to receive more data
    bool isRecvComplete;                   // true when both headers and body(optional) is completed
    bool isHeadersComplete;                // true when all headers have been received
    unordered_map<string, string> headers; // contains all the heders in key, value pairs
    string body;                           // contains the body of the request(optional)
    size_t contentLength = 0;              // contains the length of the body if present
    string httpMethod;                     // contains string method
    string resPath;                        // contains resource path
    string httpVersion;                    // contains http version

#ifdef PLATFORM_WINDOWS
    ClientState() : client_socket(nullptr), bytesSent(0), waitingForSend(false), waitingForRecv(true) {}
    explicit ClientState(SOCKET csocket) : client_socket(csocket), bytesSent(0), waitingForSend(false), waitingForRecv(true),
                                           isRecvComplete(false), isHeadersComplete(false)
    {
    }
#elif defined(PLATFORM_LINUX)
    ClientState() : client_socket(-1), bytesSent(0), waitingForSend(false), waitingForRecv(true) {}
    explicit ClientState(int csocket) : client_socket(csocket), bytesSent(0), waitingForSend(false), waitingForRecv(true),
                                        isRecvComplete(false), isHeadersComplete(false)
    {
    }
#elif defined(PLATFORM_MAC)
    ClientState() : client_socket(-1), bytesSent(0), waitingForSend(false), waitingForRecv(true) {}
    explicit ClientState(int csocket) : client_socket(csocket), bytesSent(0), waitingForSend(false), waitingForRecv(true),
                                        isRecvComplete(false), isHeadersComplete(false)
    {
    }
#endif

    void clear()
    {
        bytesSent = 0;
        waitingForSend = false;
        waitingForRecv = true;
        isRecvComplete = false;
        isHeadersComplete = false;
        body.clear();
        httpMethod.clear();
        resPath.clear();
        httpVersion.clear();
        sendBuffer.clear();
        recvBuffer.clear();
        headers.clear();
    }
};

class TCPServer
{
public:
    using Callback = std::function<void(int)>;
    using httpRecvCallback = std::function<void(ClientState &)>;

    virtual bool initialize(int port, const std::string &ipAddress = "0.0.0.0") = 0; // Initialize with port and IP
    virtual void start() = 0;                                                        // Start the server and listen for connections
    virtual ~TCPServer() = default;
    // Set callback functions for various events
    virtual void setRecvCallback(httpRecvCallback recvCallback) = 0;
    virtual void setSendCallback(Callback sendCallback) = 0;
    virtual void setAcceptCallback(Callback acceptCallback) = 0;
    virtual void setErrorCallback(Callback errorCallback) = 0;
    virtual void setListenCallback(Callback listenCallback) = 0;
    virtual void SendResponse(ClientState &client_state) = 0;
};

// Factory function to create the appropriate server instance based on the OS
TCPServer *createServer();

#endif // TCPSERVER_H

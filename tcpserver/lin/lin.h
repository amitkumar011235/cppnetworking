#ifndef LINSERVER_H
#define LINSERVER_H

#include "../tcpserver.h"
#include <string>
#include <sys/epoll.h>
#include "ctpl_stl.h"
#include <unordered_map>

struct ClientState {
    int client_socket;
    std::string recvBuffer;  // Buffer for holding incoming data
    std::string sendBuffer;  // Buffer for holding outgoing data
    size_t bytesSent;        // Tracks how many bytes have been sent
    bool waitingForSend;     // True if waiting to send more data
    bool waitingForRecv;     // True if waiting to receive more data

    ClientState() : client_socket(-1), bytesSent(0), waitingForSend(false), waitingForRecv(true) {}
    explicit ClientState(int csocket) : client_socket(csocket), bytesSent(0), waitingForSend(false), waitingForRecv(true){}
};

class LinServer : public TCPServer
{
public:
    LinServer();
    virtual ~LinServer();

    bool initialize(int port, const std::string &ip_address = "0.0.0.0") override;
    void start() override;
    
private:
    int server_fd;                        // File descriptor for the server socket
    int epoll_fd;                         // File descriptor for epoll
    // Map to store state for each client
    std::unordered_map<int, ClientState> clientStates; 
    std::thread epollThread;              // dedicated thread for epoll_wait()
    static ctpl::thread_pool threadPool;  // Static thread pool shared by all instances
    static int determineThreadPoolSize(); // Helper to determine the thread pool size
    void handleRecv(int client_socket);
    void handleSend(int client_socket);
    void handleError(int client_socket);
    void epollLoop(); // The dedicated epoll thread
    void prepareAndSendResponse(int client_socket, const std::string &response);
    std::string getDateTimeHTMLResponse();
    bool isRequestComplete(int client_socket);
    void enableKeepAlive(int sockfd);
    void handleClient(int client_socket);
    bool setSocketNonBlocking(int socketId);
};

#endif // LINSERVER_H

#ifndef LINSERVER_H
#define LINSERVER_H

#include "../tcpserver.h"
#include <string>
#include <sys/epoll.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../ctpl_stl.h"
#include <unordered_map>

using namespace std;

class LinServer : public TCPServer
{
public:
    LinServer();
    virtual ~LinServer();

    bool initialize(int port, const std::string &ip_address = "0.0.0.0") override;
    void start() override;
    void setRecvCallback(httpRecvCallback recvCallback);
    void setSendCallback(Callback sendCallback);
    void setAcceptCallback(Callback acceptCallback);
    void setErrorCallback(Callback errorCallback);
    void setListenCallback(Callback listenCallback);
    void SendResponse(ClientState& client_state);
    
private:
    int server_fd;                        // File descriptor for the server socket
    int epoll_fd;                         // File descriptor for epoll
     SSL_CTX* ctx;                        // openssl context
    // Map to store state for each client
    std::unordered_map<int, ClientState> clientStates; 
    std::thread epollThread;              // dedicated thread for epoll_wait()
    static ctpl::thread_pool threadPool;  // Static thread pool shared by all instances

    // Callback function pointers
    httpRecvCallback receiveCallback;
    Callback sendCallback;
    Callback acceptCallback;
    Callback errorCallback;
    Callback listenCallback;


    static int determineThreadPoolSize(); // Helper to determine the thread pool size
    void handleRecv(int client_socket);
    void handleSend(int client_socket);
    void handleError(int client_socket);
    void epollLoop(); // The dedicated epoll thread
    void prepareAndSendResponse(int client_socket, const std::string &response);
    std::string getDateTimeHTMLResponse();
    bool isRequestComplete(int client_socket);
    void enableKeepAlive(int sockfd);
    //void handleClient(int client_socket);
    bool setSocketNonBlocking(int socketId);
    size_t parseHeaders(int client_socket, const string& data);
    void   parseRequestLine(int client_socket, const std::string& request_line);

    //openssl functions : 
    void init_openssl();
    void cleanup_openssl();
    SSL_CTX* create_ssl_context();
    void configure_context();

};

#endif // LINSERVER_H

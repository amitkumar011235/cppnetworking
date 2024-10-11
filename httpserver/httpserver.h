#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <vector>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <string>
#include "httprequest.h"
#include "httpresponse.h"
#include "../tcpserver/tcpserver.h"

using namespace std;

class HTTPServer {
public:
    HTTPServer();

    // Add route for GET requests
    HTTPServer& get(const std::string& path, std::function<void(HTTPRequest&, HTTPResponse&)> handler);

    // Add route for POST requests
    HTTPServer& post(const std::string& path, std::function<void(HTTPRequest&, HTTPResponse&)> handler);

    // Start the server
    void listen(int port, std::function<void()> onStart);
    void sendResponseData(ClientState& state, const string& response_body);
    

private:
    // Handle the incoming client connections
   // void handleClient(int clientSocket);

    using listenCallback =  function<void()> ;
    TCPServer* tcpServer;        // Your underlying TCP server
    std::unordered_map<std::string, std::function<void(HTTPRequest&, HTTPResponse&)>> getHandlers;
    std::unordered_map<std::string, std::function<void(HTTPRequest&, HTTPResponse&)>> postHandlers;
    listenCallback onStartListening;

    // Callback functions to handle HTTP requests
    void onRecv(ClientState& clientState);
    void onSend(int clientSocket);
    void onAccept(int clientSocket);
    void onError(int clientSocket);
    void onListen(int clientSocket);

    void setListenCallback(listenCallback onListenCallback); //this will be set by user when calling listen api
    

   
};

#endif // HTTPSERVER_H

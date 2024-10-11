// #ifndef CONNECTION_H
// #define CONNECTION_H

// #include <string>
// #include <functional>
// #include <netinet/in.h>  // For sockaddr_in
// #include "HTTPRequest.h" // Include the HTTPRequest class
// #include "HTTPResponse.h" // Include the HTTPResponse class
// #include "HTTPServer.h"   // Include the HTTPServer class

// class HTTPServer;

// class Connection {
// public:
//     // Public members
//     int clientSocket;
//     sockaddr_in clientAddress;
//     std::string recvBuffer;  // To store the received data
//     HTTPRequest* request;
//     HTTPResponse* response;
//     HTTPServer* server;  // Reference to the HTTP server

//     // Constructor
//     Connection(int sock, sockaddr_in addr, HTTPServer* srv);

//     // Destructor
//     ~Connection();

//     // Called once recv is completed and all data is received
//     void onRecvComplete();

//     // Called once send is completed
//     void onSendComplete();

//     // Called on error
//     void onError();

//     // Clear request and response to free memory
//     void clear();
// };

// #endif  // CONNECTION_H

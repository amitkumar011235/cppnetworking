// #include "connection.h"
// #include <unistd.h>  // For close()
// #include <sys/socket.h>  // For send()
// #include <cstring>  // For memset()

// // Constructor
// Connection::Connection(int sock, sockaddr_in addr, HTTPServer* srv)
//     : clientSocket(sock), clientAddress(addr), request(nullptr), response(nullptr), server(srv) {}

// // Destructor
// Connection::~Connection() {
//     // Clean up memory and close socket
//     if (request) {
//         delete request;
//     }
//     if (response) {
//         delete response;
//     }
//     close(clientSocket);
// }

// // Called when recv is completed and all data is received
// void Connection::onRecvComplete() {
//     // Parse the HTTP data to create HTTPRequest object
//     request = new HTTPRequest(recvBuffer);
    
//     // Create an HTTPResponse object to send a response
//     response = new HTTPResponse(clientSocket);

//     // Call the handler for the path
//     server->handleRequest(request, response);
// }

// // Called when send is completed (if needed)
// void Connection::onSendComplete() {
//     // Handle any actions needed after sending response
// }

// // Called on error
// void Connection::onError() {
//     // Handle error: cleanup, close socket, etc.
//     clear();
// }

// // Clears the request and response objects
// void Connection::clear() {
//     if (request) {
//         delete request;
//         request = nullptr;
//     }
//     if (response) {
//         delete response;
//         response = nullptr;
//     }
// }

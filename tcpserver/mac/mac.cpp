#include "mac.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

// Buffer size for receiving messages
#define BUFFER_SIZE 1024

MacServer::MacServer() : server_fd(-1), new_socket(-1) {}

MacServer::~MacServer() {
    if (new_socket != -1) {
        close(new_socket);
    }
    if (server_fd != -1) {
        close(server_fd);
    }
}

bool MacServer::initialize(int port, const std::string& ip_address) {
    struct sockaddr_in address;
    int opt = 1;
    
    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Socket creation failed!" << std::endl;
        return false;
    }

    // Set socket options to reuse address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt failed!" << std::endl;
        return false;
    }

    // Set up the server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // all available IP addresses
    address.sin_port = htons(port);          // Set the port number

    // Bind the socket to the IP address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed!" << std::endl;
        return false;
    }

    // Start listening for connections
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed!" << std::endl;
        return false;
    }

    std::cout << "Server initialized on " << ip_address << ":" << port << std::endl;
    return true;
}

void MacServer::start() {
    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);
    char buffer[BUFFER_SIZE] = {0};

    std::cout << "Waiting for connections..." << std::endl;

    // Accept an incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr*)&client_address, &addrlen)) < 0) {
        std::cerr << "Accept failed!" << std::endl;
        return;
    }

    std::cout << "Connection accepted!" << std::endl;

    // Read the request from the client
    int valread = read(new_socket, buffer, BUFFER_SIZE);
    std::cout << "Received request: " << std::endl << buffer << std::endl;

    // Prepare the HTTP response
    const char* http_response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 44\r\n"
        "\r\n"
        "<html><body><h1>Hello from server!</h1></body></html>";

    // Send the HTTP response back to the client
    send(new_socket, http_response, strlen(http_response), 0);
    std::cout << "HTTP response sent!" << std::endl;

    // Close the connection
    close(new_socket);
    new_socket = -1;
}

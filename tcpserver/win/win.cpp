#include "win.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

bool WinServer::initialize(int port, const std::string &ipAddress)
{

    WSADATA wsaData;
    int wsInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsInit != 0)
    {
        std::cerr << "WSAStartup failed: " << wsInit << std::endl;
        return false;
    }

    this->port = port;
    this->ipAddress = ipAddress;
    std::cout << "Initialized server with IP: " << ipAddress << " on port: " << port << std::endl;

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return false;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    // Convert IP address string to binary form and assign to address
    if (inet_pton(AF_INET, ipAddress.c_str(), &address.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or address not supported" << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return false;
    }

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed" << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return false;
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed" << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return false;
    }
    std::cout << "Server is listening on " << ipAddress << ":" << port << std::endl;
    return true;
}

void WinServer::start()
{
    // Accept a connection
    struct sockaddr_in clientAddress;
    int addrlen = sizeof(clientAddress);
    SOCKET new_socket = accept(server_fd, (struct sockaddr *)&clientAddress, &addrlen);
    if (new_socket == INVALID_SOCKET)
    {
        std::cerr << "Accept failed" << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return;
    }

    std::cout << "Connection established!" << std::endl;

    // Receive data from the client
    char buffer[1024] = {0};
    int valread = recv(new_socket, buffer, sizeof(buffer), 0);
    if (valread > 0)
    {
        std::cout << "Received: " << buffer << std::endl;
    }

    // Send response to the client
    const char *http_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 44\r\n"
        "\r\n"
        "<html><body><h1>Hello from server!</h1></body></html>";

    send(new_socket, http_response, strlen(http_response), 0);
    std::cout << "Hello message sent" << std::endl;

    // Close the sockets
    closesocket(new_socket);
}

WinServer::~WinServer()
{
    // Clean up
    if (server_fd != INVALID_SOCKET)
    {
        closesocket(server_fd);
    }
    WSACleanup();
}

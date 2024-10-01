#include "win.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

// Initialize the static thread pool using the hardware concurrency
ctpl::thread_pool WinServer::threadPool(WinServer::determineThreadPoolSize());

// Helper function to determine the number of threads based on hardware concurrency
int WinServer::determineThreadPoolSize()
{
    int concurrency = std::thread::hardware_concurrency();
    return (concurrency > 0) ? concurrency : 4; // Default to 4 if hardware concurrency is unavailable
}

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

void WinServer::handleClient(SOCKET clientSocket)
{
    char buffer[1024] = {0};

    // Continue to receive data until the client closes the connection
    while (true)
    {
        // Clear the buffer before receiving new data
        memset(buffer, 0, sizeof(buffer));

        // Receive data from the client
        int valread = recv(clientSocket, buffer, sizeof(buffer), 0);

        // If the read is less than or equal to 0, the client has closed the connection
        if (valread <= 0)
        {
            std::cerr << "Client disconnected or error occurred. Closing connection." << std::endl;
            break; // Exit the loop to close the connection
        }

        // Log the received data
        std::cout << "Received: " << buffer << std::endl;

        // Prepare the HTTP response
        std::string http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " +
            std::to_string(valread) + "\r\n"
                                      "\r\n" +
            std::string(buffer, valread); // Echo the received data

        // Send the HTTP response back to the client
        send(clientSocket, http_response.c_str(), http_response.length(), 0);

        std::cout << "Echo response sent!" << std::endl;
    }

    // Close the connection
    closesocket(clientSocket);
    std::cout << "Client connection closed." << std::endl;
}

void WinServer::start()
{
    struct sockaddr_in clientAddress;
    int addrlen = sizeof(clientAddress);
    char buffer[1024] = {0};

    std::cout << "Waiting for connections..." << std::endl;

    while (true)
    { // Infinite loop to continuously accept connections
        // Accept a connection
        SOCKET client_socket = accept(server_fd, (struct sockaddr *)&clientAddress, &addrlen);
        if (client_socket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue; // Continue to the next iteration to accept new connections
        }

        std::cout << "Connection established!" << std::endl;

        // Submit a task to the thread pool to handle the client
        threadPool.push([this, client_socket](int thread_id)
                        { this->handleClient(client_socket); });
    }
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

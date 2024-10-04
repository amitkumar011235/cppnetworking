#include "win.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

// Initialize the static thread pool using the hardware concurrency
ctpl::thread_pool WinServer::threadPool(WinServer::determineThreadPoolSize());

WinServer::WinServer() : server_fd(INVALID_SOCKET), iocpHandle(NULL) {}


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

    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (iocpHandle == NULL) {
        std::cerr << "Failed to create IOCP handle!" << std::endl;
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

     // Start IOCP worker threads (number of threads based on hardware concurrency)
    unsigned int numThreads = determineThreadPoolSize();

    for (unsigned int i = 0; i < numThreads; ++i) {
         // Submit a task to the thread pool to handle the client
        threadPool.push([this](int thread_id){
            this->workerThread();
        });
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

bool WinServer::setSocketNonBlocking(SOCKET socket_fd) {
    u_long mode = 1;  // 1 to enable non-blocking mode
    if (ioctlsocket(socket_fd, FIONBIO, &mode) != 0) {
        std::cerr << "Error setting socket to non-blocking mode!" << std::endl;
        return false;
    }

    std::cout << "Socket set to non-blocking mode." << std::endl;
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

         // Set the client socket to non-blocking
        setSocketNonBlocking(client_socket);

        // Create a PerIoData structure for the client
        PerIoData* ioData = new PerIoData();
        ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));
        ioData->clientSocket = client_socket;
        ioData->wsabuf.buf = ioData->buffer;
        ioData->wsabuf.len = BUFFER_SIZE;

        // Associate the client socket with the IOCP
        CreateIoCompletionPort((HANDLE)client_socket, iocpHandle, (ULONG_PTR)client_socket, 0);

        // Post an initial recv request
        DWORD flags = 0;
        WSARecv(client_socket, &ioData->wsabuf, 1, NULL, &flags, &ioData->overlapped, NULL);

        // Submit a task to the thread pool to handle the client
        // threadPool.push([this, client_socket](int thread_id)
        //                 { this->handleClient(client_socket); });
    }
}


void WinServer::workerThread() {
    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    LPOVERLAPPED overlapped;

    while (true) {
        BOOL result = GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &completionKey, &overlapped, INFINITE);
        if (!result) {
            std::cerr << "GetQueuedCompletionStatus failed!" << std::endl;
            continue;
        }

        SOCKET clientSocket = (SOCKET)completionKey;
        PerIoData* ioData = (PerIoData*)overlapped;

        if (bytesTransferred == 0) {
            std::cerr << "Client disconnected." << std::endl;
            closesocket(clientSocket);
            delete ioData;
            continue;
        }

        if (ioData->bytesRecv == 0) {
            // Handle received data
            handleRecv(ioData);
        } else {
            // Handle sent data
            handleSend(ioData);
        }
    }
}

void WinServer::handleRecv(PerIoData* ioData) {
    std::cout << "Received data from client: " << std::string(ioData->buffer, ioData->bytesRecv) << std::endl;

    // Echo the data back to the client
    ioData->wsabuf.buf = ioData->buffer;
    ioData->wsabuf.len = ioData->bytesRecv;
    ioData->bytesRecv = 0; // Reset recv size

    // Post a send request
    DWORD flags = 0;
    WSASend(ioData->clientSocket, &ioData->wsabuf, 1, NULL, flags, &ioData->overlapped, NULL);
}

void WinServer::handleSend(PerIoData* ioData) {
    std::cout << "Sent data to client." << std::endl;

    // Post another recv request
    ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));
    ioData->wsabuf.buf = ioData->buffer;
    ioData->wsabuf.len = BUFFER_SIZE;

    DWORD flags = 0;
    WSARecv(ioData->clientSocket, &ioData->wsabuf, 1, NULL, &flags, &ioData->overlapped, NULL);
}

void WinServer::handleError(PerIoData* ioData) {
    std::cerr << "Socket error, closing connection." << std::endl;
    closesocket(ioData->clientSocket);
    delete ioData;
}


WinServer::~WinServer()
{
    // Clean up
    if (server_fd != INVALID_SOCKET)
    {
        closesocket(server_fd);
    }
    CloseHandle(iocpHandle);
    WSACleanup();
}


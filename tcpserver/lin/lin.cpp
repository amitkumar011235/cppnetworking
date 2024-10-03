#include "lin.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <mutex>
#include <fcntl.h>

using namespace std;
// Buffer size for receiving messages
#define BUFFER_SIZE 1024

#define MAX_EVENTS 10

mutex coutMutex; // mutex to sync the output on console . we can comment the lock code.

LinServer::LinServer() : server_fd(-1), epoll_fd(-1) {}

LinServer::~LinServer()
{
    if (server_fd != -1)
    {
        close(server_fd);
    }
    if (epollThread.joinable())
    {
        epollThread.join();
    }
}

// Initialize the static thread pool using the hardware concurrency
ctpl::thread_pool LinServer::threadPool(LinServer::determineThreadPoolSize());

// Helper function to determine the number of threads based on hardware concurrency
int LinServer::determineThreadPoolSize()
{
    int concurrency = std::thread::hardware_concurrency();
    return (concurrency > 0) ? concurrency : 4; // Default to 4 if hardware concurrency is unavailable
}

bool LinServer::setSocketNonBlocking(int socketId)
{
    int flags = fcntl(socketId, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "Error getting socket flags!" << std::endl;
        return false;
    }

    flags |= O_NONBLOCK;
    if (fcntl(socketId, F_SETFL, flags) == -1)
    {
        std::cerr << "Error setting socket to non-blocking!" << std::endl;
        return false;
    }

    std::cout << "Socket set to non-blocking mode." << std::endl;
    return true;
}

void LinServer::handleClient(int client_socket)
{
    char buffer[BUFFER_SIZE] = {0};

    // Continue to receive data until the client closes the connection
    while (true)
    {
        // Clear the buffer before receiving new data
        memset(buffer, 0, sizeof(buffer));

        // Receive data from the client
        int valread = read(client_socket, buffer, BUFFER_SIZE);

        // If the read is less than or equal to 0, the client has closed the connection
        if (valread <= 0)
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "Client disconnected or error occurred. Closing connection." << std::endl;
            break; // Exit the loop to close the connection
        }

        // Log the received data
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "Received: " << buffer << std::endl;
        }

        // Prepare the HTTP response
        std::string http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " +
            std::to_string(valread) + "\r\n"
                                      "\r\n" +
            std::string(buffer, valread); // Echo the received data

        // Send the HTTP response back to the client
        send(client_socket, http_response.c_str(), http_response.length(), 0);

        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "Echo response sent!" << std::endl;
        }
    }

    // Close the connection
    close(client_socket);
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "Client connection closed." << std::endl;
    }
}

bool LinServer::initialize(int port, const std::string &ip_address)
{
    cout << "initialising the server..." << endl;
    struct sockaddr_in address;
    int opt = 1;

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "Socket creation failed!" << std::endl;
        return false;
    }

    // Set socket options to reuse address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        std::cerr << "setsockopt failed!" << std::endl;
        return false;
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        std::cerr << "Epoll creation failed!" << std::endl;
        return false;
    }

    // Set up the server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // all available ip addresses
    address.sin_port = htons(port);       // Set the port number

    // Bind the socket to the IP address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed!" << std::endl;
        return false;
    }

    // Start listening for connections
    if (listen(server_fd, 3) < 0)
    {
        std::cerr << "Listen failed!" << std::endl;
        return false;
    }

    std::cout << "Server initialized on " << ip_address << ":" << port << std::endl;
    return true;
}

void LinServer::start()
{
    epollThread = std::thread(&LinServer::epollLoop, this);
    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);
    char buffer[BUFFER_SIZE] = {0};
    int client_socket = -1;

    std::cout << "Waiting for connections..." << std::endl;

    while (true)
    { // Infinite loop to accept connections continuously
        // Accept an incoming connection
        if ((client_socket = accept(server_fd, (struct sockaddr *)&client_address, &addrlen)) < 0)
        {
            std::cerr << "Accept failed! Error: " << strerror(errno) << std::endl;
            continue; // Continue to the next iteration to accept new connections
        }
        std::cout << "Connection accepted!" << std::endl;

        // Set client socket to non-blocking
        setSocketNonBlocking(client_socket);

        // Add the new client socket to the epoll set for monitoring
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
        ev.data.fd = client_socket;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &ev);

        std::cout << "New client connected and added to epoll." << std::endl;
    }
}

void LinServer::epollLoop()
{
    struct epoll_event events[MAX_EVENTS];

    while (true)
    {
        int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < event_count; i++)
        {
            int fd = events[i].data.fd;

            if (events[i].events & EPOLLIN)
            {
                // Push to thread pool for receiving data
                threadPool.push([this, fd](int thread_id)
                        { this->handleRecv(fd); });
            }
            else if (events[i].events & EPOLLOUT)
            {
                // Push to thread pool for sending data
                threadPool.push([this, fd](int thread_id)
                        { this->handleSend(fd); });
            }
            else if (events[i].events & EPOLLERR)
            {
                // Handle errors
                threadPool.push([this, fd](int thread_id)
                        { this->handleError(fd); });
            }
        }
    }
}


void LinServer::handleRecv(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytesRead = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytesRead > 0) {
        std::cout << "Received data from client: " << std::string(buffer, bytesRead) << std::endl;

        // Handle received data, echo it back or process it
    } else if (bytesRead == 0) {
        std::cout << "Client disconnected." << std::endl;
        close(client_socket);
    } else {
        std::cerr << "Error in recv." << std::endl;
    }
}

void LinServer::handleSend(int client_socket) {
    // Example: Sending data back to the client (echo or HTTP response)
    const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello";
    send(client_socket, response, strlen(response), 0);
}

void LinServer::handleError(int client_socket) {
    std::cerr << "Socket error, closing connection." << std::endl;
    close(client_socket);
}

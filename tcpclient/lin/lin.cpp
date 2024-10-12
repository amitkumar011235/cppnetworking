#include <iostream>
#include "lin.h"
#include "../tcpclient.h"
#include <fcntl.h>

LinClient::LinClient() : clientSocket(-1) {}

LinClient::~LinClient()
{
    if (clientSocket != -1)
    {
        close(clientSocket);
    }
}


bool LinClient::initialize()
{
    // No explicit initialization required on Linux
    return true;
}

void LinClient::setSendSocketBuffer(int send_size)
{
    if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, &send_size, sizeof(send_size)) == 0)
    {
        std::cout << "Send buffer size set to " << send_size << " bytes" << std::endl;
    }
    else
    {
        std::cerr << "Error setting send buffer size!" << std::endl;
    }
}

void LinClient::setRecvSocketBuffer(int recv_size)
{
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, &recv_size, sizeof(recv_size)) == 0)
    {
        std::cout << "Receive buffer size set to " << recv_size << " bytes" << std::endl;
    }
    else
    {
        std::cerr << "Error setting receive buffer size!" << std::endl;
    }
}

void LinClient::getSendSocketBuffer()
{
    int send_size;
    socklen_t optlen = sizeof(send_size);
    if (getsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, &send_size, &optlen) == 0)
    {
        std::cout << "Current send buffer size: " << send_size << " bytes" << std::endl;
    }
    else
    {
        std::cerr << "Error getting send buffer size!" << std::endl;
    }
}

void LinClient::getRecvSocketBuffer()
{
    int recv_size;
    socklen_t optlen = sizeof(recv_size);
    if (getsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, &recv_size, &optlen) == 0)
    {
        std::cout << "Current receive buffer size: " << recv_size << " bytes" << std::endl;
    }
    else
    {
        std::cerr << "Error getting receive buffer size!" << std::endl;
    }
}

void LinClient::setTimeout(int seconds)
{
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    // Set the timeout for receive and send operations
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) < 0)
    {
        std::cerr << "Failed to set receive timeout!" << std::endl;
    }

    if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout)) < 0)
    {
        std::cerr << "Failed to set send timeout!" << std::endl;
    }

    std::cout << "Timeout set to " << seconds << " seconds." << std::endl;
}

bool LinClient::setSocketNonBlocking()
{
    int flags = fcntl(clientSocket, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "Error getting socket flags!" << std::endl;
        return false;
    }

    flags |= O_NONBLOCK;
    if (fcntl(clientSocket, F_SETFL, flags) == -1)
    {
        std::cerr << "Error setting socket to non-blocking!" << std::endl;
        return false;
    }

    std::cout << "Socket set to non-blocking mode." << std::endl;
    return true;
}

bool LinClient::start(const std::string &ip, unsigned short port)
{
    // Create a socket for connecting to server
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        std::cerr << "Error at socket(): " << strerror(errno) << std::endl;
        return false;
    }

    // Setup the sockaddr_in structure
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &clientService.sin_addr);
    clientService.sin_port = htons(port);

    // Connect to server
    int result = connect(clientSocket, (struct sockaddr *)&clientService, sizeof(clientService));
    if (result == -1)
    {
        std::cerr << "connect() failed: " << strerror(errno) << std::endl;
        close(clientSocket);
        return false;
    }

    std::cout << "Successfully connected to server: " << ip << ":" << port << std::endl;
    return true;
}

bool LinClient::sendDataold(const std::string &data)
{
    ssize_t result = send(clientSocket, data.c_str(), data.length(), 0);
    if (result == -1)
    {
        std::cerr << "send() failed: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "Bytes Sent: " << result << std::endl;
    return true;
}

// Utility function to send all data to ensure all bytes are sent
bool LinClient::sendAll(const char *buffer, size_t length)
{
    ssize_t totalSent = 0;
    ssize_t bytesLeft = length;
    ssize_t sent;

    while (totalSent < length)
    {
        sent = send(clientSocket, buffer + totalSent, bytesLeft, 0);
        if (sent == -1)
        {
            std::cerr << "send() failed: " << strerror(errno) << std::endl;
            return false;
        }
        totalSent += sent;
        bytesLeft -= sent;
    }
    return true;
}

// sendData function with optional length parameter
bool LinClient::sendData(const std::string &data, int length, const std::string &delimiter)
{
    if (length > 0)
    {
        // If length is provided, send Content-Length header
        std::string header = "Content-Length: " + std::to_string(length) + "\r\n\r\n";
        if (!sendAll(header.c_str(), header.size()))
        {
            return false;
        }

        // Send the content in chunks according to length
        if (!sendAll(data.c_str(), length))
        {
            return false;
        }
    }
    else
    {
        // Stream-based sending
        if (!sentDelimiterHeader)
        {
            // Send the delimiter header at the very beginning
            std::string delimiterHeader = "delimiter: " + delimiter + "\r\n\r\n";
            if (!sendAll(delimiterHeader.c_str(), delimiterHeader.size()))
            {
                return false;
            }
            sentDelimiterHeader = true;
        }

        // Send the actual data
        if (!sendAll(data.c_str(), data.size()))
        {
            return false;
        }

        // If -1 is passed as length, send the delimiter to signify the end of the stream
        if (length == -1)
        {
            if (!sendAll(delimiter.c_str(), delimiter.size()))
            {
                return false;
            }
            sentDelimiterHeader = false; // Reset for future streams
        }
    }

    return true;
}

// Function to receive data based on Content-Length or Delimiter
std::string LinClient::recvData()
{
    std::string header = receiveHeader();
    std::string content;

    // Check if the header contains Content-Length
    size_t contentLengthPos = header.find("Content-Length:");
    if (contentLengthPos != std::string::npos)
    {
        // Extract the content length
        size_t lengthStart = contentLengthPos + std::strlen("Content-Length: ");
        size_t lengthEnd = header.find("\r\n", lengthStart);
        int contentLength = std::stoi(header.substr(lengthStart, lengthEnd - lengthStart));

        // Read content based on content-length
        content = receiveFixedLengthContent(contentLength);
    }
    else
    {
        // Look for Delimiter header
        size_t delimiterPos = header.find("Delimiter:");
        if (delimiterPos != std::string::npos)
        {
            // Extract the delimiter string
            size_t delimiterStart = delimiterPos + std::strlen("Delimiter: ");
            size_t delimiterEnd = header.find("\r\n", delimiterStart);
            std::string delimiter = header.substr(delimiterStart, delimiterEnd - delimiterStart);

            // Read content until delimiter is found
            content = receiveUntilDelimiter(delimiter);
        }
        else
        {
            std::cerr << "No Content-Length or Delimiter header found" << std::endl;
            return "";
        }
    }

    return content;
}

// Receive the header section from the client (until the double \r\n\r\n)
std::string LinClient::receiveHeader()
{
    std::string header;
    char buffer[1024];
    ssize_t bytesReceived;
    bool headerComplete = false;

    while (!headerComplete)
    {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            std::cerr << "Error receiving header: " << strerror(errno) << std::endl;
            return "";
        }

        header.append(buffer, bytesReceived);

        // Check if we have reached the end of the header (which is \r\n\r\n)
        if (header.find("\r\n\r\n") != std::string::npos)
        {
            headerComplete = true;
        }
    }

    // Remove the \r\n\r\n at the end of the header
    return header.substr(0, header.find("\r\n\r\n") + 2); // +2 to keep a \r\n after the header
}

// Receive fixed-length content based on the Content-Length header
std::string LinClient::receiveFixedLengthContent(int contentLength)
{
    std::string content;
    content.reserve(contentLength);
    char *buffer = new char[4096];

    int totalBytesReceived = 0;
    while (totalBytesReceived < contentLength)
    {
        ssize_t bytesReceived = recv(clientSocket, buffer, std::min(4096, contentLength - totalBytesReceived), 0);
        if (bytesReceived <= 0)
        {
            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
            break;
        }
        content.append(buffer, bytesReceived);
        totalBytesReceived += bytesReceived;
    }

    delete[] buffer;
    return content;
}

// Receive content until the delimiter is found
std::string LinClient::receiveUntilDelimiter(const std::string &delimiter)
{
    std::string content;
    char buffer[4096];
    ssize_t bytesReceived;

    while (true)
    {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
            break;
        }

        content.append(buffer, bytesReceived);

        // Check if the delimiter is found in the content
        if (content.find(delimiter) != std::string::npos)
        {
            // Remove the delimiter from the content
            size_t delimiterPos = content.find(delimiter);
            content = content.substr(0, delimiterPos);
            break;
        }
    }

    return content;
}

std::string LinClient::receiveData(size_t maxLength)
{
    char *buffer = new char[maxLength];
    ssize_t result = recv(clientSocket, buffer, maxLength, 0);
    if (result > 0)
    {
        std::string data(buffer, result);
        delete[] buffer;
        return data;
    }
    else if (result == 0)
    {
        std::cerr << "Connection closed by server." << std::endl;
    }
    else
    {
        std::cerr << "recv() failed: " << strerror(errno) << std::endl;
    }
    delete[] buffer;
    return "";
}

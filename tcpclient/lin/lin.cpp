#include <iostream>
#include "lin.h"
#include "../tcpclient.h"

LinClient::LinClient() : clientSocket(-1) {}

LinClient::~LinClient() {
    if (clientSocket != -1) {
        close(clientSocket);
    }
}

bool LinClient::initialize() {
    // No explicit initialization required on Linux
    return true;
}

void LinClient::setSendSocketBuffer(int send_size) {
    if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, &send_size, sizeof(send_size)) == 0) {
        std::cout << "Send buffer size set to " << send_size << " bytes" << std::endl;
    } else {
        std::cerr << "Error setting send buffer size!" << std::endl;
    }
}

void LinClient::setRecvSocketBuffer(int recv_size) {
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, &recv_size, sizeof(recv_size)) == 0) {
        std::cout << "Receive buffer size set to " << recv_size << " bytes" << std::endl;
    } else {
        std::cerr << "Error setting receive buffer size!" << std::endl;
    }
}

void LinClient::getSendSocketBuffer() {
    int send_size;
    socklen_t optlen = sizeof(send_size);
    if (getsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, &send_size, &optlen) == 0) {
        std::cout << "Current send buffer size: " << send_size << " bytes" << std::endl;
    } else {
        std::cerr << "Error getting send buffer size!" << std::endl;
    }
}

void LinClient::getRecvSocketBuffer() {
    int recv_size;
    socklen_t optlen = sizeof(recv_size);
    if (getsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, &recv_size, &optlen) == 0) {
        std::cout << "Current receive buffer size: " << recv_size << " bytes" << std::endl;
    } else {
        std::cerr << "Error getting receive buffer size!" << std::endl;
    }
}


void LinClient::setTimeout(int seconds) {
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    // Set the timeout for receive and send operations
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        std::cerr << "Failed to set receive timeout!" << std::endl;
    }

    if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        std::cerr << "Failed to set send timeout!" << std::endl;
    }

    std::cout << "Timeout set to " << seconds << " seconds." << std::endl;
}

bool LinClient::setSocketNonBlocking() {
    int flags = fcntl(clientSocket, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Error getting socket flags!" << std::endl;
        return false;
    }

    flags |= O_NONBLOCK;
    if (fcntl(clientSocket, F_SETFL, flags) == -1) {
        std::cerr << "Error setting socket to non-blocking!" << std::endl;
        return false;
    }

    std::cout << "Socket set to non-blocking mode." << std::endl;
    return true;
}

bool LinClient::start(const std::string& ip, unsigned short port) {
    // Create a socket for connecting to server
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error at socket(): " << strerror(errno) << std::endl;
        return false;
    }

    // Setup the sockaddr_in structure
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &clientService.sin_addr);
    clientService.sin_port = htons(port);

    // Connect to server
    int result = connect(clientSocket, (struct sockaddr*)&clientService, sizeof(clientService));
    if (result == -1) {
        std::cerr << "connect() failed: " << strerror(errno) << std::endl;
        close(clientSocket);
        return false;
    }

    std::cout << "Successfully connected to server: " << ip << ":" << port << std::endl;
    return true;
}

bool LinClient::sendData(const std::string& data) {
    ssize_t result = send(clientSocket, data.c_str(), data.length(), 0);
    if (result == -1) {
        std::cerr << "send() failed: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "Bytes Sent: " << result << std::endl;
    return true;
}

std::string LinClient::receiveData(size_t maxLength) {
    char* buffer = new char[maxLength];
    ssize_t result = recv(clientSocket, buffer, maxLength, 0);
    if (result > 0) {
        std::string data(buffer, result);
        delete[] buffer;
        return data;
    } else if (result == 0) {
        std::cerr << "Connection closed by server." << std::endl;
    } else {
        std::cerr << "recv() failed: " << strerror(errno) << std::endl;
    }
    delete[] buffer;
    return "";
}

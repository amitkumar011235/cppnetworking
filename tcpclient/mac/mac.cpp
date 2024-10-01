#include <iostream>
#include "mac.h"
#include "../tcpclient.h"

MacClient::MacClient() : clientSocket(-1) {}

MacClient::~MacClient() {
    if (clientSocket != -1) {
        close(clientSocket);
    }
}

bool MacClient::initialize() {
    // No explicit initialization required on macOS
    return true;
}

bool MacClient::start(const std::string& ip, unsigned short port) {
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

bool MacClient::sendData(const std::string& data) {
    ssize_t result = send(clientSocket, data.c_str(), data.length(), 0);
    if (result == -1) {
        std::cerr << "send() failed: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "Bytes Sent: " << result << std::endl;
    return true;
}

std::string MacClient::receiveData(size_t maxLength) {
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

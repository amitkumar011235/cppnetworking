#include <iostream>
#include "win.h"
#include "../tcpclient.h"

WinClient::WinClient() : clientSocket(INVALID_SOCKET) {}

WinClient::~WinClient() {
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
    }
    WSACleanup();
}

bool WinClient::initialize() {
    // Initialize Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }
    return true;
}

bool WinClient::start(const std::string& ip, unsigned short port) {
    // Create a socket for connecting to server
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    // Setup the sockaddr_in structure
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &clientService.sin_addr);
    clientService.sin_port = htons(port);

    // Connect to server
    int result = connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService));
    if (result == SOCKET_ERROR) {
        std::cerr << "connect() failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    std::cout << "Successfully connected to server: " << ip << ":" << port << std::endl;
    return true;
}

bool WinClient::sendData(const std::string& data) {
    int result = send(clientSocket, data.c_str(), data.length(), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "send() failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    std::cout << "Bytes Sent: " << result << std::endl;
    return true;
}

std::string WinClient::receiveData(size_t maxLength) {
    char* buffer = new char[maxLength];
    int result = recv(clientSocket, buffer, maxLength, 0);
    if (result > 0) {
        std::string data(buffer, result);
        delete[] buffer;
        return data;
    } else if (result == 0) {
        std::cerr << "Connection closed by server." << std::endl;
    } else {
        std::cerr << "recv() failed: " << WSAGetLastError() << std::endl;
    }
    delete[] buffer;
    return "";
}

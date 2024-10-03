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


void WinClient::setTimeout(int seconds) {
    timeout = seconds * 1000; // Convert to milliseconds

    // Set the timeout for receive and send operations
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        std::cerr << "Failed to set receive timeout!" << std::endl;
    }

    if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        std::cerr << "Failed to set send timeout!" << std::endl;
    }

    std::cout << "Timeout set to " << seconds << " seconds." << std::endl;
}

void WinClient::setSendSocketBuffer(int send_size) {
    if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (char*)&send_size, sizeof(send_size)) == 0) {
        std::cout << "Send buffer size set to " << send_size << " bytes" << std::endl;
    } else {
        std::cerr << "Error setting send buffer size!" << std::endl;
    }
}

void WinClient::setRecvSocketBuffer(int recv_size) {
   if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, (char*)&recv_size, sizeof(recv_size)) == 0) {
        std::cout << "Receive buffer size set to " << recv_size << " bytes" << std::endl;
    } else {
        std::cerr << "Error setting receive buffer size!" << std::endl;
    }
}

void WinClient::getSendSocketBuffer() {
    int send_size;
    int optlen = sizeof(send_size);
    if (getsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (char*)&send_size, &optlen) == 0) {
        std::cout << "Current send buffer size: " << send_size << " bytes" << std::endl;
    } else {
        std::cerr << "Error getting send buffer size!" << std::endl;
    }
}

void WinClient::getRecvSocketBuffer() {
    int recv_size;
    int optlen = sizeof(recv_size);
    if (getsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, (char*)&recv_size, &optlen) == 0) {
        std::cout << "Current receive buffer size: " << recv_size << " bytes" << std::endl;
    } else {
        std::cerr << "Error getting receive buffer size!" << std::endl;
    }
}

bool WinClient::setSocketNonBlocking() {
    u_long mode = 1;  // 1 to enable non-blocking mode
    if (ioctlsocket(clientSocket, FIONBIO, &mode) != 0) {
        std::cerr << "Error setting socket to non-blocking mode!" << std::endl;
        return false;
    }

    std::cout << "Socket set to non-blocking mode." << std::endl;
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

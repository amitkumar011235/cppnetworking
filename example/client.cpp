#include <iostream>
#include "tcpclient.h"



int main() {
    // Create the TCP server using the factory function
    TCPClient* client = createClient();

    if (!client->initialize()) {
        return 1;
    }

    std::string serverIp = "35.154.194.14";
    unsigned short serverPort = 8080;

    if (!client->start(serverIp, serverPort)) {
        return 1;
    }

    // Send data to server
    std::string message = "image1.png";
    if (!client->sendData(message)) {
        return 1;
    }

    // client->getSendSocketBuffer();
    // client->getRecvSocketBuffer();


    // Receive data from server
    std::string response = client->recvData();
    if (!response.empty()) {
        std::cout << "Received from server: " << response << std::endl;
    }

    return 0;
}

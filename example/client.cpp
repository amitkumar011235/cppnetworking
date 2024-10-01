#include <iostream>
#include "tcpclient.h"



int main() {
    // Create the TCP server using the factory function
    TCPClient* client = createClient();

    if (!client->initialize()) {
        return 1;
    }

    std::string serverIp = "127.0.0.1";
    unsigned short serverPort = 8080;

    if (!client->start(serverIp, serverPort)) {
        return 1;
    }

    // Send data to server
    std::string message = "Hello, Server!";
    if (!client->sendData(message)) {
        return 1;
    }

    // Receive data from server
    std::string response = client->receiveData();
    if (!response.empty()) {
        std::cout << "Received from server: " << response << std::endl;
    }

    return 0;
}

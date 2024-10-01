#include "tcpserver.h"
#include <iostream>

int main() {
    // Create the TCP server using the factory function
    TCPServer* server = createServer();
    
    if (server) {
        // Initialize the server with a specific port and (optional) IP address
        server->initialize(8080);  // Default is localhost if no IP is provided

        std::cout << "Server initialized." << std::endl;
        
        // Start the server
        server->start();

        // Clean up
        delete server;
    } else {
        std::cerr << "Failed to create server instance." << std::endl;
    }

    return 0;
}

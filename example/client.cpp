#include <iostream>
#include "tcpclient.h"


/*

http protocol 

client  : 

request -> 

GET /users HTTP/1.1 \r\n
Host: www.example.com\r\n
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
Accept:
text/html,application/xhtml+xml,application/xml;q=0.9,image/webp
Connection: keep-alive
content-length: 5\r\n
\r\n
hello hi therer



response :

 HTTP/1.1 200 OK\r\n
 Date: Tue, 06 Oct 2024 12:34:56 GMT\r\n
 Server: Apache/2.4.1 (Unix)\r\n
 Content-Type: text/html
 Content-Length: 1024\r\n
 \r\n
 <html>
 <head><title>Example Page</title></head>
 <body><



server : 


*/



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

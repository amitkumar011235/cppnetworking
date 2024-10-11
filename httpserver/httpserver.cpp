#include "httpserver.h"
#include <cstring>

HTTPServer::HTTPServer()
{
    // Initialize any server state here if necessary
    // set the callbacks at the tcp layer i.e. recv callback , send callback , listen callback , accept callback and error callback
    tcpServer = createServer();
    tcpServer->setErrorCallback([this](int clientSocket)
                                { onError(clientSocket); });
    tcpServer->setAcceptCallback([this](int clientSocket)
                                 { onAccept(clientSocket); });
    tcpServer->setListenCallback([this](int clientSocket)
                                 { onListen(clientSocket); });
    tcpServer->setRecvCallback([this](ClientState &clientState)
                               { onRecv(clientState); });
    tcpServer->setSendCallback([this](int clientSocket)
                               { onSend(clientSocket); });

    cout << "http server initialization done" << endl;
}

void HTTPServer::sendResponseData(ClientState &state, const string &response_body)
{
    state.bytesSent = 0;
    state.sendBuffer = response_body;
    tcpServer->SendResponse(state);
}

void HTTPServer::onRecv(ClientState &clientState)
{
    cout<<"recvd data headers are : "<<endl;
    for(auto& itr : clientState.headers){
        cout<<"header : "<<itr.first<<" value : "<<itr.second<<endl;
    }
    cout<<"recvd data : path -> "<<clientState.resPath<<endl;
    cout<<"recvd data : http method -> "<<clientState.httpMethod<<endl;


    HTTPRequest req(clientState);
    HTTPResponse res(clientState, [this](ClientState &state, const std::string &response_body)
                     { sendResponseData(state, response_body); });

    if (req.getMethod() == "GET" && getHandlers.find(req.getPath()) != getHandlers.end())
    {
        getHandlers[req.getPath()](req, res);
    }
    else if (req.getMethod() == "POST" && postHandlers.find(req.getPath()) != postHandlers.end())
    {
        postHandlers[req.getPath()](req, res);
    }
    else
    {
        cout<<"inside the 404 not found block of http server"<<endl;
        // Handle 404 Not Found
        res.send("404 Not Found");
    }
}

void HTTPServer::onSend(int clientSocket)
{
    cout << "Sent data to client socket: " << clientSocket << std::endl;
}

void HTTPServer::onAccept(int clientSocket)
{
    cout << "Accepted new connection on socket: " << clientSocket << std::endl;
}

void HTTPServer::onError(int clientSocket)
{
    cout << "Error on socket: " << clientSocket << std::endl;
}

void HTTPServer::onListen(int clientSocket)
{
    cout<<"came to the http server listen function"<<endl;
    // calling the user given function once server is ready to start accepting the connections.
    onStartListening();
}

HTTPServer &HTTPServer::get(const std::string &path, std::function<void(HTTPRequest &, HTTPResponse &)> handler)
{
    cout << "addding the getter " << endl;
    getHandlers[path] = handler;
    return *this;
}

HTTPServer &HTTPServer::post(const std::string &path, std::function<void(HTTPRequest &, HTTPResponse &)> handler)
{
    cout << "addding the post " << endl;

    postHandlers[path] = handler;
    return *this;
}

void HTTPServer::listen(int port, listenCallback onStart)
{
    cout << "starting to listen  " << endl;
    setListenCallback(onStart);

    tcpServer->initialize(port); // Default is localhost if no IP is provided

    std::cout << "Server initialized." << std::endl;

    // Start the server
    tcpServer->start(); // this start is the forever loop for server
}

void HTTPServer::setListenCallback(listenCallback listenCallback)
{
    this->onStartListening = listenCallback;
}

// void HTTPServer::handleClient(int clientSocket)
// {
//     char buffer[1024] = {0};
//     ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

//     if (bytesReceived > 0)
//     {
//         std::string requestStr(buffer, bytesReceived);

//         // Parse the HTTP request
//         HTTPRequest request = parseRequest(requestStr);
//         HTTPResponse response(clientSocket);

//         // Find the matching route
//         for (const auto &route : routes)
//         {
//             if (route.method == request.method && route.path == request.path)
//             {
//                 route.handler(request, response);
//                 return;
//             }
//         }

//         // If no route matches, send 404
//         response.send404();
//     }

//     // Close the client socket after handling
//     close(clientSocket);
// }

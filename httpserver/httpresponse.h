#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <string>
#include "../tcpserver/tcpserver.h"

using namespace std;
class HTTPResponse {
public:
    // Constructor takes a reference to ClientState
    explicit HTTPResponse(ClientState& clientState, std::function<void(ClientState&, const std::string&)> sendCallback);

    // Add a header to the HTTP response
    void addHeader(const std::string& key, const std::string& value);

    // Set the HTTP status code and message
    void setStatus(int statusCode, const std::string& statusMessage);

    // Send the response body to the client
    void send(const std::string& body);

    // Send only the headers (for responses without a body)
    void send();

    //API to Send the contents of a file as the response
    void sendFile(const std::string& filePath);

private:
    ClientState& state;  // Reference to the ClientState object for socket communication
    unordered_map<std::string, std::string> responseHeaders;  // Map of headers to send
    std::function<void(ClientState&, const std::string&)> sendCallback;  // Callback function to send data. set by http server

    int statusCode = 200;  // HTTP status code
    std::string statusMessage;  // HTTP status message

    // Internal method to construct and send the response
    void sendResponse(const std::string& body);
    void sendResponse(); // Overloaded for sending without body
    
    // Helper method to determine content type from file extension
    std::string getContentType(const std::string& filePath);
};

#endif // HTTPRESPONSE_H

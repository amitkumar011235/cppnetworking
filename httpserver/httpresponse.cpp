#include <sstream>
#include "httpresponse.h"

// Constructor initializes status code and message to default (200 OK)
HTTPResponse::HTTPResponse(ClientState &clientState, std::function<void(ClientState&, const std::string&)> sendCallback)
    : state(clientState), statusCode(200), statusMessage("OK"), sendCallback(sendCallback) {}

// Add a header to the response
void HTTPResponse::addHeader(const std::string &key, const std::string &value)
{
    responseHeaders[key] = value;
}

// Set the HTTP status code and message
void HTTPResponse::setStatus(int statusCode, const std::string &statusMessage)
{
    this->statusCode = statusCode;
    this->statusMessage = statusMessage;
}

// Send the response with the body
void HTTPResponse::send(const std::string &body)
{
    sendResponse(body);
}

// Send the response without a body
void HTTPResponse::send()
{
    sendResponse();
}

// Internal method to construct and send the HTTP response with a body
void HTTPResponse::sendResponse(const std::string &body)
{
    std::ostringstream responseStream;

    // Build the status line
    responseStream << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";

    // Add the headers stored in the map
    for (const auto &header : responseHeaders)
    {
        responseStream << header.first << ": " << header.second << "\r\n";
    }

    // Add the Content-Length header if the body is not empty
    if (!body.empty())
    {
        responseStream << "Content-Length: " << body.size() << "\r\n";
    }

    // End headers section
    responseStream << "\r\n";

    // Add the response body if it's not empty
    if (!body.empty())
    {
        responseStream << body;
    }

    // Convert the response to a string
    std::string responseStr = responseStream.str();

    sendCallback(state, responseStr);  // Call the send callback function
}

// Internal method to construct and send the HTTP response without a body
void HTTPResponse::sendResponse()
{
    std::ostringstream responseStream;

    // Build the status line
    responseStream << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";

    // Add the headers stored in the map
    for (const auto &header : responseHeaders)
    {
        responseStream << header.first << ": " << header.second << "\r\n";
    }

    // End headers section
    responseStream << "\r\n";

    // Convert the response to a string
    std::string responseStr = responseStream.str();

   sendCallback(state, responseStr);  // Call the send callback function
}
#include <sstream>
#include <fstream>
#include "httpresponse.h"

using namespace std;

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


// Send the contents of a file as the HTTP response
void HTTPResponse::sendFile(const std::string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        // Send a 404 error if the file is not found
        addHeader("Content-Type", "text/plain");
        sendResponse("404 Not Found: Unable to load the requested file.");
        return;
    }

    // Read the file content into a string
    stringstream buffer;
    buffer << file.rdbuf();
    string fileContent = buffer.str();

    // Set the content type based on the file extension
    addHeader("Content-Type", getContentType(filePath));

    sendResponse(fileContent);  
}

// Helper function to determine content type based on file extension
std::string HTTPResponse::getContentType(const std::string& filePath) {
    static std::unordered_map<std::string, std::string> mimeTypes = {
        {".html", "text/html"},
        {".css", "text/css"},
        {".js", "application/javascript"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".gif", "image/gif"},
        // Add more mime types as needed
    };

    std::size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string ext = filePath.substr(dotPos);
        if (mimeTypes.find(ext) != mimeTypes.end()) {
            return mimeTypes[ext];
        }
    }

    // Default content type
    return "text/plain";
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
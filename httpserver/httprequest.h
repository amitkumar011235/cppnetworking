#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <unordered_map>
#include "../tcpserver/tcpserver.h"

class HTTPRequest {
public:
    HTTPRequest(ClientState& state) : state(state) {}

    std::string getMethod() const { return state.httpMethod; }
    std::string getPath() const { return state.resPath; }
    std::string getHeader(const std::string& key) const {
        if (state.headers.find(key) != state.headers.end()) {
            return state.headers.at(key);
        }
        return "";
    }
    std::string getBody() const { return state.body; }

private:
    ClientState& state;
};

#endif // HTTPREQUEST_H

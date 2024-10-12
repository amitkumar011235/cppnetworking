#include "../httpserver/httprequest.h"
#include "../httpserver/httpserver.h"
#include "../httpserver/httpresponse.h"

#include <iostream>


 Server Push:
 HTTP/1.1: The client must request all resources (e.g., HTML, CSS, JavaScript).
 HTTP/2: Allows the server to “push” resources to the client before it even requests them. For
 example, if the server knows that a CSS file is needed when sending an HTML file, it can
 push the CSS file immediately

using namespace std;

int main()
{
    // Create the TCP server using the factory function
    const int PORT = 8000;
    HTTPServer *server = new HTTPServer();

    if (server)
    {
        // Initialize the server with a specific port and (optional) IP address
        // server->initialize(8080);  // Default is localhost if no IP is provided

        std::cout << "Server initialized." << std::endl;

        // Start the server
        server->get("/users", [](HTTPRequest &req, HTTPResponse &res)
                    {
                     cout << "send all users from thsi route" << endl;

                        res.addHeader("header1", "value1");
                        res.setStatus(200, "ok done");
                        res.send("this is the bosy of http response"); })
            .get("/shops", [](HTTPRequest &req, HTTPResponse &res)
                 {
                     cout << "send all the shops from thsi route" << endl;
                     res.send("all shops"); })
            .get("/index.html", [](HTTPRequest &req, HTTPResponse &res)
                 {
                     cout << "send the index.html file" << endl;
                     res.sendFile("./debug" + req.getPath()); })
            .get("/tictactoe.html", [](HTTPRequest &req, HTTPResponse &res)
                 {
                     cout << "send the index.html file" << endl;
                     res.sendFile("./debug" + req.getPath()); })
            .get("/users", [](HTTPRequest &req, HTTPResponse &res)
                 {
                     cout << "send all the shops from thsi route" << endl;
                     res.send("all shops"); })
            .post("/addshop", [](HTTPRequest &req, HTTPResponse &res)
                  { res.send("shop added ssuccessfully"); })
            .listen(PORT, [&]()
                    { cout << "server started listening on port : " << PORT << endl; });
    }
    else
    {
        std::cout << "Failed to create http server instance." << std::endl;
    }

    // Clean up
    delete server;
    return 0;
}

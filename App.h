#ifndef APP_H
#define APP_H


#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <csignal>
#include <unistd.h>

#include "connections/Connection.h"
#include "responses/Request.h"
#include "responses/Response.h"

extern "C" {
#include <netdb.h>
}

class App {
    int sockfd;
    std::map<std::string, std::function<Response (const Request &)> > routes;

public:
    App() = default;

    void run(int port);

    void accept_connection(Connection &connection);

    void close_socket() const;

    void route(const std::string &path, const std::function<Response (const Request &)> &callback);

    ~App() {
        close_socket();
    }
};


#endif

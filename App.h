#ifndef APP_H
#define APP_H


#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <signal.h>
#include <unistd.h>

#include "connections/Connection.h"

extern "C" {
#include <netdb.h>
}

class App {
    int sockfd;

public:
    App() = default;

    void run(int port);

    void accept_connection(Connection &connection);

    void close_socket() const;

    ~App() {
        close_socket();
    }
};


#endif

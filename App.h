#ifndef APP_H
#define APP_H

#include <functional>
#include <map>

#include "connections/Connection.h"
#include "responses/Request.h"
#include "responses/Response.h"
#include "SslHelpers.h"

class App {
    int sockfd;
    std::map<std::string, std::function<Response (const Request &)> > routes;
    SSL_CTX *ctx = nullptr;

    bool is_ssl_enabled() const;

public:
    App() = default;

    void run(int port);

    void accept_connection(Connection &connection);

    void close_socket() const;

    void route(const std::string &path, const std::function<Response (const Request &)> &callback);

    App& enable_ssl(const std::string &cert, const std::string &key);

    ~App() {
        close_socket();
    }
};


#endif

#ifndef APP_H
#define APP_H

#include <filesystem>
#include <fstream>
#include <functional>
#include <map>

#include "connections/Connection.h"
#include "responses/Request.h"
#include "responses/Response.h"
#include "SslHelpers.h"

class App final {
    int sockfd{};
    std::map<std::string, std::function<Response (const Request &)>> routes;
    SSL_CTX *ssl_ctx = nullptr;
    std::ofstream log_file;

    [[nodiscard]] bool is_ssl_enabled() const;

public:
    App() = default;

    void run(int port);

    void accept_connection(Connection &connection) const;

    void close_socket() const;

    void route(const std::string &path, const std::function<Response (const Request &)> &callback);

    App &enable_ssl(const std::string &cert, const std::string &key);

    ~App() {
        if (this->log_file.is_open()) {
            this->log_file.close();
        }
        close_socket();
    }
};

#endif

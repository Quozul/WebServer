#ifndef APP_H
#define APP_H

#include "Router.h"
#include "SslHelpers.h"
#include "connections/Connection.h"

#include <filesystem>
#include <fstream>

class App final {
    const Router &router;
    int sockfd{};
    SSL_CTX *ssl_ctx = nullptr;
    std::ofstream log_file;
    bool is_running = true;
    bool access_logs = true;

    [[nodiscard]] bool is_ssl_enabled() const;

  public:
    explicit App(const Router &router) : router(router) {}

    void run(int port);

    void handle_client(const int &client) const;

    void accept_connection(Connection &connection) const;

    void close_socket() const;

    App &enable_ssl(const std::string &cert, const std::string &key);

    ~App() {
        if (this->log_file.is_open()) {
            this->log_file.close();
        }
        close_socket();
    }
};

#endif

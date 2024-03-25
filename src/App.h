#ifndef APP_H
#define APP_H

#include "Router.h"
#include "SslHelpers.h"
#include "clients/Client.h"

#include <filesystem>
#include <fstream>

class App final {
    const Router &router;
    int sockfd{};
    SSL_CTX *ssl_ctx = nullptr;
    std::ofstream log_file;
    bool is_running = true;
    std::map<int, std::unique_ptr<Client>> clients;
    int max_sd{};
    fd_set master_fds{}, read_fds{};

    [[nodiscard]] bool is_ssl_enabled() const;

    void add_new_client(int new_socket);

    void handle_client();

    [[nodiscard]] bool accept_new();

  public:
    explicit App(const Router &router) : router(router) {}

    void run(int port);

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

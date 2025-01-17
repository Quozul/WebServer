#ifndef APP_H
#define APP_H

#include "Router.h"
#include "SslHelpers.h"
#include "clients/Client.h"
#include "event_loops/EventLoop.h"

#include <filesystem>
#include <mutex>

class App final {
    const Router &router_;
    int sockfd{};
    SSL_CTX *ssl_ctx = nullptr;
    bool is_running = true;
    std::map<int, std::unique_ptr<Client> > clients;
    std::mutex clients_mutex_;

    [[nodiscard]] bool is_ssl_enabled() const;

    void create_ssl_client(int new_socket);

    void create_socket_client(int new_socket);

    bool handle_client(EventLoop &event_loop, int i);

    void accept_new(EventLoop &event_loop);

public:
    explicit App(const Router &router) : router_(router) {
    }

    void run(int port);

    void close_socket();

    App &enable_ssl(const std::string &cert, const std::string &key);

    ~App();
};

#endif

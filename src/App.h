#ifndef APP_H
#define APP_H

#include <filesystem>
#include <fstream>
#include <functional>
#include <map>

#include "SslHelpers.h"
#include "connections/Connection.h"
#include "responses/Request.h"
#include "responses/Response.h"

typedef std::function<Response(const Request &)> Handler;

class App final {
    int sockfd{};
    std::map<std::string, Handler> routes;
    SSL_CTX *ssl_ctx = nullptr;
    std::ofstream log_file;
    bool is_running = true;

    [[nodiscard]] bool is_ssl_enabled() const;

  public:
    App() = default;

    void run(int port);

    void handle_client(const int &client) const;

    void accept_connection(Connection &connection) const;

    void close_socket() const;

    void route(const std::string &path, const Handler &callback);

    /**
     * @throws UndefinedRoute
     * @param request
     * @return
     */
    Response handle_request(const Request &request) const;

    App &enable_ssl(const std::string &cert, const std::string &key);

    ~App() {
        if (this->log_file.is_open()) {
            this->log_file.close();
        }
        close_socket();
    }
};

class UndefinedRoute final : public std::exception {};

#endif

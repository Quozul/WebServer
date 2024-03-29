#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include "../Router.h"
#include "../parsers/RequestParser.h"
#include "Client.h"

class SocketClient : public Client {
  protected:
    RequestParser parser_;
    const Router &router_;
    const int socket_;
    bool is_connected_;

  public:
    explicit SocketClient(const int client, const Router &router) : router_(router), socket_(client) {
        is_connected_ = true;
    }

    ~SocketClient() override = default;

    void socket_read() override;

    void socket_write(const char *data, size_t size) override;

    void close_connection() override;

    bool is_active() override;
};

#endif

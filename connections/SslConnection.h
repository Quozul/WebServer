#ifndef SSLCONNECTION_H
#define SSLCONNECTION_H

#include <openssl/ssl.h>

#include "Connection.h"
#include "SocketConnection.h"


class SslConnection final : public SocketConnection{
private:
    SSL *ssl;

public:
    bool is_ready = false;

    explicit SslConnection(int client, SSL_CTX *ctx);

    ~SslConnection() override = default;

    std::string socket_read() override;

    void write_socket(const std::string &body) override;

    void close_socket() override;
};

#endif

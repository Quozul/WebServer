#ifndef SSLCONNECTION_H
#define SSLCONNECTION_H

#include <openssl/ssl.h>

#include "SocketConnection.h"


class SslConnection final : public SocketConnection {
    SSL *ssl;

public:
    explicit SslConnection(int client, SSL_CTX *ctx);

    ~SslConnection() override = default;

    [[nodiscard]] bool handshake() const;

    std::string socket_read() override;

    void write_socket(const std::string &body) override;

    void close_socket() override;
};

#endif

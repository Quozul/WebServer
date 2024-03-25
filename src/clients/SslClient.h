#ifndef SSL_CLIENT_H
#define SSL_CLIENT_H

#include "SocketClient.h"

#include <openssl/ssl.h>

class SslClient final : public SocketClient {
    SSL *ssl_{};

  public:
    explicit SslClient(const int client, const Router &router) : SocketClient(client, router) {}

    ~SslClient() override = default;

    bool handshake(SSL_CTX *ctx);

    unsigned char socket_read() override;

    void socket_write(const char *data, size_t size) override;

    void close_connection() override;
};

#endif

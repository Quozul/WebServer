#include "SslConnection.h"

#include <stdexcept>
#include <unistd.h>
#include <openssl/err.h>

#include "../tracing.h"


SslConnection::SslConnection(const int client, SSL_CTX *ctx): SocketConnection(client) {
    this->ssl = SSL_new(ctx);
    SSL_set_fd(this->ssl, this->client);
}

bool SslConnection::handshake() const {
    if (SSL_accept(this->ssl) <= 0) {
        return false;
    }

    return true;
}

std::string SslConnection::socket_read() {
    int bytes_read = 0, pending = 0;
    std::string final_buffer;

    do {
        char buffer[1024];

        if (const auto operation = SSL_read(this->ssl, buffer, 1024); operation > 0) {
            bytes_read += operation;
        } else {
            if (const auto shutdown = SSL_get_shutdown(this->ssl); shutdown == SSL_RECEIVED_SHUTDOWN || shutdown == SSL_SENT_SHUTDOWN) {
                throw std::runtime_error("Connection closed");
            }
        }

        final_buffer.append(buffer);
        pending = SSL_pending(this->ssl);
    } while (bytes_read > 0 && pending > 0);

    return final_buffer;
}

void SslConnection::write_socket(const std::string &body) {
    SSL_write(this->ssl, body.c_str(), body.size());
}

void SslConnection::close_socket() {
    SSL_shutdown(this->ssl);
    SSL_free(this->ssl);
    close(this->client);
}

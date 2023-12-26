#include "SslConnection.h"

#include <stdexcept>
#include <unistd.h>
#include <openssl/err.h>

#include "../tracing.h"

#define BUFFER_SIZE 16'384

SslConnection::SslConnection(const int client, SSL_CTX *ctx): SocketConnection(client) {
    this->ssl = SSL_new(ctx);
    SSL_set_fd(this->ssl, this->client);
    SSL_set_mode(this->ssl, SSL_MODE_AUTO_RETRY);
}

bool SslConnection::handshake() const {
    if (SSL_accept(this->ssl) <= 0) {
        return false;
    }

    return true;
}

std::string SslConnection::socket_read() {
    std::string final_buffer;

    do {
        char buffer[BUFFER_SIZE];

        const auto operation = SSL_read(this->ssl, buffer, BUFFER_SIZE);

        if (operation > 0) {
            final_buffer.append(buffer, operation);
        }

        if (SSL_get_shutdown(this->ssl) > 0) {
            throw std::runtime_error("closed connection");
        }

        const auto ssl_error = SSL_get_error(this->ssl, operation);

        if (ssl_error == SSL_ERROR_NONE) {
            // TODO: should flush before checking for pending state
            if (!SSL_has_pending(this->ssl)) {
                break;
            }
        } else if (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE) {
            const auto value = select(this->client + 1, nullptr, nullptr, nullptr, nullptr);
            tracing::info("select: {}", value);
            if (value < 0) {
                break;
            }
        } else if (ssl_error == SSL_ERROR_WANT_ASYNC_JOB
                   || ssl_error == SSL_ERROR_SYSCALL
                   || ssl_error == SSL_ERROR_SSL
                   || ssl_error == SSL_ERROR_ZERO_RETURN) {
            throw std::runtime_error("closed connection");
        } else {
            tracing::error("Unknown error {}", ssl_error);
            throw std::runtime_error("unknown error");
        }
    } while (true);

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

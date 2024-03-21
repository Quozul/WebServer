#include "SslConnection.h"

#include <openssl/err.h>
#include <stdexcept>
#include <unistd.h>

#include "../parsers/RequestParser.h"
#include "../tracing.h"

#define BUFFER_SIZE 16'384

SslConnection::SslConnection(const int client, SSL_CTX *ctx) : SocketConnection(client) {
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

int get_buffer_size(const size_t remaining) {
    constexpr size_t default_buffer_size = BUFFER_SIZE;
    const size_t buffer_size = std::min(default_buffer_size, remaining);
    const auto res = static_cast<int>(buffer_size);
    return res;
}

Request SslConnection::socket_read() {
    RequestParser parser{};
    const auto buffer = new char[BUFFER_SIZE];

    while (!parser.is_complete()) {
        const auto buffer_size = get_buffer_size(parser.remaining_bytes());

        const auto operation = SSL_read(this->ssl, buffer, buffer_size);

        if (operation > 0) {
            parser.append_content(std::string(buffer, operation));
        }

        if (SSL_get_shutdown(this->ssl) > 0) {
            throw std::runtime_error("closed connection");
        }

        const auto ssl_error = SSL_get_error(this->ssl, operation);

        if (ssl_error == SSL_ERROR_NONE) {
            // TODO: should flush before checking for pending state
            /*if (!SSL_has_pending(this->ssl)) {
                break;
            }*/
        } else if (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE) {
            const auto value = select(this->client + 1, nullptr, nullptr, nullptr, nullptr);
            tracing::info("select: {}", value);
            if (value < 0) {
                break;
            }
        } else if (ssl_error == SSL_ERROR_WANT_ASYNC_JOB || ssl_error == SSL_ERROR_SYSCALL ||
                   ssl_error == SSL_ERROR_SSL || ssl_error == SSL_ERROR_ZERO_RETURN) {
            throw std::runtime_error("closed connection");
        } else {
            tracing::error("Unknown error {}", ssl_error);
            throw std::runtime_error("unknown error");
        }
    }

    delete[] buffer;
    return parser.request;
}

void SslConnection::write_socket(const std::string &body) {
    // TODO: Send in chunks
    SSL_write(this->ssl, body.c_str(), body.size());
}

void SslConnection::close_socket() {
    SSL_shutdown(this->ssl);
    SSL_free(this->ssl);
    close(this->client);
}

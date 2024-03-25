#include "SslClient.h"

#include <openssl/err.h>
#include <spdlog/spdlog.h>

bool SslClient::handshake(SSL_CTX *ctx) {
    ssl_ = SSL_new(ctx);
    SSL_set_fd(ssl_, socket_);
    SSL_set_mode(ssl_, SSL_MODE_AUTO_RETRY);

    const int handshake_result = SSL_accept(ssl_);
    if (handshake_result <= 0) {
        int ssl_error = SSL_get_error(ssl_, handshake_result);
        if (ssl_error != SSL_ERROR_WANT_READ && ssl_error != SSL_ERROR_WANT_WRITE) {
            return false;
        }
    }

    return true;
}

unsigned char SslClient::socket_read() {
    char buffer[BUFFER_SIZE];

    auto buffer_size = get_buffer_size(parser_.remaining_bytes());
    const int bytes_received = SSL_read(ssl_, buffer, buffer_size);

    if (bytes_received > 0) {
        parser_.append_content(std::string(buffer, bytes_received));
    }

    if (SSL_get_shutdown(ssl_) > 0) {
        return DISCONNECTED;
    }

    if (!parser_.has_more()) {
        Response response_builder;
        router_.handle_request(parser_.request, response_builder);

        const std::string response = response_builder.build();

        socket_write(response.c_str(), response.size());

        const RequestParser new_parser{};
        parser_ = new_parser;
    }

    const auto ssl_error = SSL_get_error(ssl_, bytes_received);

    if (ssl_error == SSL_ERROR_NONE || ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE) {
        return STILL_HERE;
    }

    if (ssl_error == SSL_ERROR_WANT_ASYNC_JOB || ssl_error == SSL_ERROR_SYSCALL || ssl_error == SSL_ERROR_SSL ||
        ssl_error == SSL_ERROR_ZERO_RETURN) {
        return DISCONNECTED;
    }

    spdlog::error("SSL_read error: '{}'", ERR_error_string(SSL_get_error(ssl_, bytes_received), nullptr));
    return DISCONNECTED;
}

void SslClient::socket_write(const char *data, const size_t size) {
    size_t offset = 0;

    while (offset < size) {
        constexpr size_t chunk_size = BUFFER_SIZE;
        const size_t bytes_to_send = std::min(chunk_size, size - offset);
        const int bytes_sent = SSL_write(ssl_, data + offset, bytes_to_send);

        if (bytes_sent <= 0) {
            const int ssl_error = SSL_get_error(ssl_, bytes_sent);
            if (ssl_error != SSL_ERROR_WANT_READ && ssl_error != SSL_ERROR_WANT_WRITE) {
                spdlog::error("SSL_write error: '{}'", ERR_error_string(SSL_get_error(ssl_, bytes_sent), nullptr));
                break;
            }
        } else {
            offset += bytes_sent;
        }
    }
}

void SslClient::close_connection() {
    spdlog::trace("Client {} disconnected", socket_);
    SSL_shutdown(ssl_);
    SSL_free(ssl_);
    close(socket_);
}
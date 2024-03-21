#include "SslHelpers.h"

#include <openssl/err.h>

void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

SSL_CTX *create_context() {
    const SSL_METHOD *method = TLS_server_method();

    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION);
    SSL_CTX_set_mode(ctx, SSL_MODE_RELEASE_BUFFERS);
    SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);

    return ctx;
}

void configure_context(SSL_CTX *ctx, const char *cert, const char *key) {
    SSL_CTX_set_ecdh_auto(ctx, 1);

    if (SSL_CTX_use_certificate_chain_file(ctx, cert) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_set_cipher_list(ctx, "HIGH:!aNULL:!MD5:!RC4") != 1) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_options(ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
}

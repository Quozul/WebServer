#ifndef SSL_HELPERS_H
#define SSL_HELPERS_H

#include <openssl/ssl.h>
#include <openssl/err.h>

void init_openssl();

SSL_CTX *create_context();

void configure_context(SSL_CTX *ctx, const char *cert, const char *key);

#endif

extern "C" {
#include <lauxlib.h>
#include <lualib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
}

#include <lua.hpp>

#include <cstdio>
#include <cstring>
#include <thread>
#include <functional>
#include <map>
#include <csignal>

#include "servlet.h"
#include "Queue.h"

#define THREADS 1

int create_socket(int port) {
    int s;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    return s;
}

void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
}

SSL_CTX *create_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx, const char *cert, const char *key) {
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

[[noreturn]] void consume(Queue<serve> &queue) {
    while (true) {
        auto s = queue.pop();
        servelet(s);

        // Close connection
        SSL_shutdown(s.ssl);
        SSL_free(s.ssl);
        close(s.client);
    }
}

int main() {
    // Read config file
    std::map<std::string, std::string> config;
    std::ifstream file("server.conf");
    std::string line;
    size_t sep;
    while (std::getline(file, line)) {
        sep = line.find('=');
        if (sep != std::string::npos) {
            config.insert({line.substr(0, sep), line.substr(sep + 1, line.length() - sep - 2)});
        }
    }

    init_openssl();
    SSL_CTX *ctx = create_context();

    configure_context(ctx, config.at("cert").c_str(), config.at("key").c_str());
    config.erase("cert");
    config.erase("key");

    int port = std::stoi(config.at("port"));
    int sockfd = create_socket(port);
    config.erase("port");

    // Initialize lua
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_base(L);
    luaopen_table(L);
    luaopen_io(L);
    luaopen_string(L);
    luaopen_math(L);

    // Start response threads
    Queue<serve> queue;
    for (unsigned int i = 0; i < THREADS; ++i) {
        std::thread handler(std::bind(&consume, std::ref(queue)));
        handler.detach();
    }

    std::signal(SIGPIPE, SIG_IGN); // Disable SIGPIPE

    /* Handle connections */
    while (true) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        SSL *ssl;

        int client = accept(sockfd, (struct sockaddr *) &addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            continue;
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            continue;
        }

        serve s(ssl, L, addr, client, config);
        queue.push(s);
    }

    close(sockfd);
    SSL_CTX_free(ctx);
    cleanup_openssl();
    lua_close(L);
}
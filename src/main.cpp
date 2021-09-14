extern "C" {
#include <lauxlib.h>
#include <lualib.h>

#ifdef _WIN32
// Required for Windows compilation
#include <openssl/applink.c>
#endif
}

#include <cstdio>
#include <cstring>
#include <thread>
#include <functional>
#include <map>
#include <csignal>

#include "servlet.hpp"
#include "Queue.hpp"
#include "socket.hpp"

[[noreturn]] void consume(Queue<Connection> &queue) {
    while (true) {
        auto s = queue.pop();
        servlet(s);

        // Close connection
        SSL_shutdown(s.ssl);
        SSL_free(s.ssl);
        sockClose(s.client);
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

    // Initialize socket server with SSL support
    init_openssl();
    SSL_CTX *ctx = create_context();

    configure_context(ctx, config.at("cert").c_str(), config.at("key").c_str());

    sockInit();
    int port = std::stoi(config.at("port"));
    int sockfd = create_socket(port);

#ifndef _WIN32
    std::signal(SIGPIPE, SIG_IGN); // Disable SIGPIPE
#endif

    // Remove unneeded keys from configuration
    config.erase("port");
    config.erase("cert");
    config.erase("key");

    // Initialize lua
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_base(L);
    luaopen_table(L);
    luaopen_io(L);
    luaopen_string(L);
    luaopen_math(L);

    // Set global LUA functions
    lua_pushcfunction(L, setResponseHeader);
    lua_setglobal(L, "setResponseHeader");

    lua_pushcfunction(L, sendResponseBody);
    lua_setglobal(L, "sendResponseBody");

    lua_pushcfunction(L, sendResponseHeaders);
    lua_setglobal(L, "sendResponseHeaders");

    lua_pushcfunction(L, getRequestHeaders);
    lua_setglobal(L, "getRequestHeaders");

    lua_pushcfunction(L, getRequestParams);
    lua_setglobal(L, "getRequestParams");

    // Start response threads
    Queue<Connection> queue;
    int threads = std::stoi(config.at("threads"));
    for (unsigned int i = 0; i < threads; ++i) {
        std::thread handler(std::bind(&consume, std::ref(queue)));
        handler.detach();
    }
    config.erase("threads");

    /* Handle connections */
    while (true) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        SSL *ssl;

        int client = accept(sockfd, (struct sockaddr *) &addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);

            SSL_shutdown(ssl);
            SSL_free(ssl);
            sockClose(client);
        } else {
            Connection s(ssl, L, client, &config);
            servlet(s);

            SSL_shutdown(s.ssl);
            SSL_free(s.ssl);
            sockClose(s.client);
            //queue.push(s);
        }
    }

    sockClose(sockfd);
    sockQuit();
    SSL_CTX_free(ctx);
    cleanup_openssl();
    lua_close(L);
}

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
#include <filesystem>

#include "servlet.hpp"
#include "Queue.hpp"
#include "socket.hpp"

#include "tao/json.hpp"

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

// TODO: Use CLI arguments to override configuration
int main() {
    std::cout << "Reading configuration..." << std::endl;

#ifdef _WIN32
    std::string config_path = "C:\\Program Data\\quozuldotweb.json";
#else
    std::string config_path;
    if (getuid()) {
        std::cout << "Not running as root" << std::endl;
        config_path = std::string(getenv("HOME")) + "/.quozuldotweb.json";
    } else {
        config_path = "/etc/quozuldotweb.json";
    }
#endif

    if (!std::filesystem::exists(config_path)) {
        std::cout << "Configuration file not found" << std::endl;
        return EXIT_FAILURE;
    }

    // Read config file
    const tao::json::value config = tao::json::from_file(config_path);

    const char *cert = config.at("cert").get_string().c_str();
    const char *key = config.at("key").get_string().c_str();
    const std::string server_path = config.at("server").get_string();
    const unsigned int port = config.at("port").get_unsigned();
    const unsigned int threads = config.at("threads").get_unsigned();

    std::cout << "Starting OpenSSL..." << std::endl;
    // Initialize socket server_path with SSL support
    init_openssl();
    SSL_CTX *ctx = create_context();

    configure_context(ctx, cert, key);

    sockInit();
    int sockfd = create_socket(port);

#ifndef _WIN32
    std::signal(SIGPIPE, SIG_IGN); // Disable SIGPIPE
#endif

    std::cout << "Starting Lua..." << std::endl;
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

    std::cout << "Starting threads..." << std::endl;
    // Start response threads
    Queue<Connection> queue;
    for (unsigned int i = 0; i < threads; ++i) {
        std::thread handler(std::bind(&consume, std::ref(queue)));
        handler.detach();
    }

    std::cout << "Server listening!" << std::endl;
    /* Handle connections */
    while (true) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        SSL *ssl;

        const int client = accept(sockfd, (struct sockaddr *) &addr, &len);
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
            Connection s(ssl, L, client, &server_path);
            queue.push(s);

            // Single threaded request handling
            /*servlet(s);
            SSL_shutdown(s.ssl);
            SSL_free(s.ssl);
            sockClose(s.client);*/
        }
    }

    sockClose(sockfd);
    sockQuit();
    SSL_CTX_free(ctx);
    cleanup_openssl();
    lua_close(L);
    return EXIT_SUCCESS;
}

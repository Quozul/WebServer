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
#include <sstream>
#include <string>
#include <regex>

#include "servlet.hpp"
#include "Queue.hpp"
#include "socket.hpp"

#include "tao/json.hpp"

[[noreturn]] void consume(Queue<Connection> &queue) {
    while (true) {
        Connection s = queue.pop();
        servlet(s);

        // Close connection
        SSL_shutdown(s.ssl);
        SSL_free(s.ssl);
        sockClose(s.client);
    }
}

// TODO: Use CLI arguments to override configuration
int main() {
#ifdef _WIN32
    std::string config_path = "C:\\ProgramData\\quozuldotweb.json";
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
        std::cout << "Configuration file not found\nLooked at: ";
        std::cout << config_path << std::endl;
        return EXIT_FAILURE;
    }

    // Read config file
    const tao::json::value config = tao::json::from_file(config_path);

    const char *cert = config.at("cert").get_string().c_str();
    const char *key = config.at("key").get_string().c_str();
    const std::string server_path = config.at("server").get_string();
    const unsigned int port = config.at("port").get_unsigned();
    const unsigned int threads = config.at("threads").get_unsigned();
    const std::string mime_types_path = config.at("mime_types").get_string();

    // Load mime.types
    std::map<std::string, std::string> mime_types;
    std::ifstream infile(mime_types_path);

    std::regex rgx("\\t+");

    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);

        if (line.starts_with('#') || line.length() == 0) continue;

        std::vector<std::string> elems = split(line, '\t');
        if (elems.size() < 2) continue;
        std::vector<std::string> extensions = split(elems[1], ' ');
        for (const std::string &ext: extensions) {
            mime_types.insert({ext, elems[0]});
        }
    }

#ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(1, 1), &wsa_data);
#endif

    // Prepare cache
    CacheMap cache;

    // Initialize socket server_path with SSL support
    init_openssl();
    SSL_CTX *ctx = create_context();

    SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION);
    SSL_CTX_set_mode(ctx, SSL_MODE_RELEASE_BUFFERS);
    SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);

    configure_context(ctx, cert, key);

    int sockfd = create_socket(port);

#ifndef _WIN32
    bool t = true;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int));

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
            Connection s(ssl, L, client, &server_path, mime_types, cache);
            queue.push(s);

            // Single threaded request handling
            /*servlet(s);
            SSL_shutdown(s.ssl);
            SSL_free(s.ssl);
            sockClose(s.client);*/
        }
    }

    std::cout << "Closing server." << std::flush;
    sockClose(sockfd);
#ifdef _WIN32
    WSACleanup();
#endif
    std::cout << "." << std::flush;
    SSL_CTX_free(ctx);
    std::cout << "." << std::flush;
    cleanup_openssl();
    std::cout << "." << std::flush;
    lua_close(L);
    std::cout << "." << std::flush;
    exit(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

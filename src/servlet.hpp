#define NOMINMAX

#ifndef WEBSERVER_SERVLET_HPP
#define WEBSERVER_SERVLET_HPP

extern "C" {
#include <openssl/ssl.h>
#include <openssl/err.h>

#ifdef _WIN32
#ifndef _WIN32_WINNT

#define _WIN32_WINNT 0x0501

#endif

#include <winsock2.h>
#include <Ws2tcpip.h>

#else

#include <unistd.h>

#endif
}

#include <lua.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <filesystem>

#include "Response.hpp"
#include "Request.hpp"
#include "luaFunctions.hpp"

#define READ_SIZE 1024

typedef struct serve {
    serve(SSL *s, lua_State *l, int c, std::map<std::string, std::string> *o) :
            ssl(s), L(l), client(c), config(o) {};

    SSL *ssl;
    lua_State *L;
    int client;
    std::map<std::string, std::string> *config;
} serve;

void servlet(serve &s);

std::string readString(const serve &s);

void serveString(const serve &s, const std::string &str);

void serveCharArray(const serve &s, const char *res, size_t len);

void serveFile(Response &response, std::string &path, serve &s);

void serveLua(Response &response, Request &request, std::string &path, serve &s);

#endif //WEBSERVER_SERVLET_HPP

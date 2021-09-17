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

#include "HttpMessages/Response.hpp"
#include "HttpMessages/Request.hpp"
#include "luaFunctions.hpp"
#include "utils.hpp"

#define READ_SIZE 1024

typedef struct Connection {
    Connection(SSL *s, lua_State *l, const int c, const std::string *p, const std::map<std::string, std::string> &m) :
            ssl(s), L(l), client(c), path(p), mime_types(m) {};

    SSL *ssl;
    lua_State *L;
    const int client;
    const std::string *path;
    const std::map<std::string, std::string> &mime_types;
} Connection;

void servlet(Connection &s);

std::string readString(const Connection &s);

void serveFile(Response &response, std::string &path, Connection &s);

void serveLua(Response &response, Request &request, std::string &path, Connection &s);

#endif //WEBSERVER_SERVLET_HPP

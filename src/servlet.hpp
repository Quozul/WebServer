#define NOMINMAX

#ifndef WEBSERVER_SERVLET_HPP
#define WEBSERVER_SERVLET_HPP

extern "C" {
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <winsock2.h>
#include <Ws2tcpip.h>

#define stat _stat

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
#include <chrono>
#include <utility>

#include "HttpMessages/Response.hpp"
#include "HttpMessages/Request.hpp"
#include "luaFunctions.hpp"
#include "utils.hpp"
#define READ_SIZE 1024

typedef struct Cache {
    explicit Cache(const std::string &path, char *d, std::streampos s) {
        struct stat result{};

        if (stat(path.c_str(), &result) == 0) {
            this->mod_time = result.st_mtime;
        }

        this->size = s;
        this->data = d;
    };

    char *data;
    long mod_time;
    std::streampos size;
} Cache;

typedef std::map<std::string, Cache *> CacheMap;
typedef std::pair<std::string, Cache *> CachePair;

typedef struct Connection {
    Connection(SSL *s, const int c, const std::string *p,
               const std::map<std::string, std::string> &m, CacheMap &a, const bool u) :
            ssl(s), client(c), path(p), mime_types(m), cache(a), use_cache(u) {};

    SSL *ssl;
    const int client;
    const std::string *path;
    const std::map<std::string, std::string> &mime_types;
    CacheMap &cache;
    const bool use_cache;
} Connection;

void servlet(Connection &s);

std::string readString(const Connection &s);

void serveFile(Response &response, const std::string &path, Connection &s);

void serveLua(Response &response, Request &request, std::string &path, Connection &s);

#endif //WEBSERVER_SERVLET_HPP

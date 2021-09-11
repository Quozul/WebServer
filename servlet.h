#ifndef WEBSERVER_SERVLET_H
#define WEBSERVER_SERVLET_H

extern "C" {
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unistd.h>
}

#include <lua.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <filesystem>

#include "Response.h"
#include "Request.h"

#define READ_SIZE 1024

void servelet(SSL* ssl, lua_State* L, struct sockaddr_in& addr, int& client);
void serveLua(Response &response, Request &request, std::filesystem::path &path, SSL* ssl, lua_State * L);

typedef struct serve {
    serve(SSL* s, lua_State* l, struct sockaddr_in& a, int& c): ssl(s), L(l), addr(a), client(c) {};

    SSL* ssl;
    lua_State* L;
    struct sockaddr_in& addr;
    int& client;
} serve;

#endif //WEBSERVER_SERVLET_H

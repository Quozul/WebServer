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

typedef struct serve {
    serve(SSL* s, lua_State* l, struct sockaddr_in& a, int c, std::map<std::string, std::string>& o): ssl(s), L(l), addr(a), client(c), config(o) {};

    SSL* ssl;
    lua_State* L;
    struct sockaddr_in& addr;
    int client;
    std::map<std::string, std::string>& config;
} serve;

void servelet(serve& s);
void serveFile(Response &response, std::string &path);
void serveLua(Response &response, Request &request, std::string &path, serve s);

#endif //WEBSERVER_SERVLET_H

#ifndef WEBSERVER_LUAFUNCTIONS_HPP
#define WEBSERVER_LUAFUNCTIONS_HPP

#define NOMINMAX

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

#include <lua.hpp>
#include <iostream>

#include "Request.hpp"
#include "Response.hpp"

#define READ_SIZE 1024

// TODO: Remove either static or non-static duplicate of this function
static void serveCharArray(SSL* ssl, const char *res, size_t len) {
    char *buf = new char[READ_SIZE];

    for (size_t i = 0; i < len; i += READ_SIZE) {
        size_t end = std::min(i + READ_SIZE, len);
        size_t l = end - i;
        std::memcpy(buf, res + i, l);
        SSL_write(ssl, buf, l);
    }

    delete[] buf;
}

// TODO: Remove either static or non-static duplicate of this function
static void serveString(SSL* ssl, std::string &str) {
    // Convert to char* then send response
    size_t len = str.length();
    char *buf = new char[READ_SIZE];
    const char *res = str.c_str();

    for (size_t i = 0; i < len; i += READ_SIZE) {
        size_t end = std::min(i + READ_SIZE, len);
        size_t l = end - i;
        std::memcpy(buf, res + i, l);
        SSL_write(ssl, buf, l);
    }

    delete[] buf;
}

static int setResponseHeader(lua_State *L) {
    auto *response = static_cast<Response *>(lua_touserdata(L, 1));
    const char *key = lua_tostring(L, 2);
    const char *value = lua_tostring(L, 3);

    response->setHeader(std::string(key), std::string(value));

    return 1;
}

static int sendResponseBody(lua_State *L) {
    auto *response = static_cast<Response *>(lua_touserdata(L, 1));
    size_t len;
    const char *content = lua_tolstring(L, 2, &len);

    serveCharArray(response->getSSL(), content, len);

    return 1;
}

static int sendResponseHeaders(lua_State *L) {
    auto *response = static_cast<Response *>(lua_touserdata(L, 1));

    std::string headers = response->getHeadersAsString();
    response->headers_sent = true;
    serveString(response->getSSL(), headers);

    return 1;
}

static int buildLuaTable(lua_State *L, std::map<std::string, std::string> &map) {
    lua_newtable(L);
    int top = lua_gettop(L);

    std::map<std::string, std::string>::iterator it, end;

    for (it = map.begin(); it != map.end(); ++it) {
        const char *key = it->first.c_str();
        const char *value = it->second.c_str();
        lua_pushlstring(L, key, it->first.size());
        lua_pushlstring(L, value, it->second.size());
        lua_settable(L, top);
    }

    return top;
}

/**
 * Parse and returns the request headers to lua
 * @param L
 * @return
 */
static int getRequestHeaders(lua_State *L) {
    auto *request = static_cast<Request *>(lua_touserdata(L, 1));

    request->parseHeaders();
    std::map<std::string, std::string> headers = request->getHeaders();
    int top = buildLuaTable(L, headers);

    lua_pushvalue(L, top);

    return 1;
}

/**
 * Parse and returns the request parameters to lua
 * @param L
 * @return
 */
static int getRequestParams(lua_State *L) {
    auto *request = static_cast<Request *>(lua_touserdata(L, 1));

    request->parseParams();
    std::map<std::string, std::string> params = request->getParameters();
    int top = buildLuaTable(L, params);

    lua_pushvalue(L, top);

    return 1;
}

#endif //WEBSERVER_LUAFUNCTIONS_HPP

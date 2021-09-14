#ifndef WEBSERVER_LUAFUNCTIONS_HPP
#define WEBSERVER_LUAFUNCTIONS_HPP

#define NOMINMAX

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

#include <lua.hpp>
#include <iostream>
#include <cstring>
#include <regex>
#include <sstream>

#include "Request.hpp"
#include "Response.hpp"

#define READ_SIZE 1024

void serveCharArray(SSL *ssl, const char *res, size_t len);

void serveString(SSL *ssl, std::string &str);

int setResponseHeader(lua_State *L);

int sendResponseBody(lua_State *L);

int sendResponseHeaders(lua_State *L);

int buildLuaTable(lua_State *L, std::map<std::string, std::string> &map);

/**
 * Parse and returns the request headers to lua
 * @param L
 * @return
 */
int getRequestHeaders(lua_State *L);

/**
 * Parse and returns the request parameters to lua
 * @param L
 * @return
 */
int getRequestParams(lua_State *L);

// Source: https://gist.github.com/arthurafarias/56fec2cd49a32f374c02d1df2b6c350f
std::string decodeURIComponent(std::string encoded);

std::string encodeURIComponent(std::string decoded);

#endif //WEBSERVER_LUAFUNCTIONS_HPP
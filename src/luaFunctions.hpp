#ifndef WEBSERVER_LUAFUNCTIONS_HPP
#define WEBSERVER_LUAFUNCTIONS_HPP

#include <lua.hpp>
#include <iostream>
#include <cstring>
#include <sstream>

#include "HttpMessages/Request.hpp"
#include "HttpMessages/Response.hpp"
#include "utils.hpp"

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

#endif //WEBSERVER_LUAFUNCTIONS_HPP

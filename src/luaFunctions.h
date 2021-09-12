#ifndef WEBSERVER_LUAFUNCTIONS_H
#define WEBSERVER_LUAFUNCTIONS_H

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

#include <lua.hpp>
#include <iostream>

#include "Request.h"
#include "Response.h"


static int setResponseHeader(lua_State *L) {
    auto *response = static_cast<Response *>(lua_touserdata(L, 1));
    const char *key = lua_tostring(L, 2);
    const char *value = lua_tostring(L, 3);

    response->setHeader(std::string(key), std::string(value));

    return 1;
}

static int buildLuaTable(lua_State* L, std::map<std::string, std::string>& map) {
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

#endif //WEBSERVER_LUAFUNCTIONS_H

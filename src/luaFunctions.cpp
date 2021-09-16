#include "luaFunctions.hpp"

int setResponseHeader(lua_State *L) {
    auto *response = static_cast<Response *>(lua_touserdata(L, 1));
    const char *key = lua_tostring(L, 2);
    const char *value = lua_tostring(L, 3);

    response->setHeader(std::string(key), std::string(value));

    return 1;
}

int sendResponseBody(lua_State *L) {
    auto *response = static_cast<Response *>(lua_touserdata(L, 1));
    size_t len;
    const char *content = lua_tolstring(L, 2, &len);

    serveCharArray(response->getSSL(), content, len);

    return 1;
}

int sendResponseHeaders(lua_State *L) {
    auto *response = static_cast<Response *>(lua_touserdata(L, 1));
    response->sendHeaders();
    return 1;
}

int buildLuaTable(lua_State *L, std::map<std::string, std::string> &map) {
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

int getRequestHeaders(lua_State *L) {
    auto *request = static_cast<Request *>(lua_touserdata(L, 1));

    request->parseHeaders();
    std::map<std::string, std::string> headers = request->getHeaders();
    int top = buildLuaTable(L, headers);

    lua_pushvalue(L, top);

    return 1;
}

int getRequestParams(lua_State *L) {
    auto *request = static_cast<Request *>(lua_touserdata(L, 1));

    request->parseParams();
    std::map<std::string, std::string> params = request->getParameters();
    int top = buildLuaTable(L, params);

    lua_pushvalue(L, top);

    return 1;
}

std::string decodeURIComponent(std::string encoded) {
    std::string decoded = encoded;
    std::smatch sm;
    std::string haystack;

    int dynamicLength = decoded.size() - 2;

    if (decoded.size() < 3) return decoded;

    for (int i = 0; i < dynamicLength; i++) {
        haystack = decoded.substr(i, 3);

        if (std::regex_match(haystack, sm, std::regex("%[0-9A-F]{2}"))) {
            haystack = haystack.replace(0, 1, "0x");
            std::string rc = {(char) std::stoi(haystack, nullptr, 16)};
            decoded = decoded.replace(decoded.begin() + i, decoded.begin() + i + 3, rc);
        }

        dynamicLength = decoded.size() - 2;
    }

    return decoded;
}

/*std::string encodeURIComponent(std::string decoded) {
    std::ostringstream oss;
    std::regex r("[!'\\(\\)*-.0-9A-Za-z_~]");

    for (char &c: decoded) {
        if (std::regex_match(std::string(c), r)) {
            oss << c;
        } else {
            oss << "%" << std::uppercase << std::hex << (0xff & c);
        }
    }
    return oss.str();
}*/
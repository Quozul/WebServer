#include "servlet.h"

void servelet(serve& s) {
    // Read request
    int read, pending;
    unsigned long received = 0;
    std::string requestString;
    char *buffer = new char[READ_SIZE];
    do {
        read = SSL_read(s.ssl, buffer, READ_SIZE);
        pending = SSL_pending(s.ssl);

        if (read > 0) {
            received += read;
            std::string str(buffer);
            requestString += str.substr(0, read);
        }

        if (pending == 0) break;
    } while (true);
    delete[] buffer;

    // Parse request
    Request request(requestString);
    Response response;

    std::string reqPath = request.getPath();

    std::filesystem::path path = std::filesystem::path(s.config.at("server"));
    path += reqPath;

    // If file exists
    if (std::filesystem::exists(path)) {
        if (std::filesystem::is_regular_file(path)) {
            std::ifstream file(path);
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            response.setHeader("Content-Length", std::to_string(contents.length()));
            response.setBody(contents);
        } else if (std::filesystem::is_directory(path)) {
            // If lua file exists
            path += "index.lua";
            if (std::filesystem::exists(path)) {
                serveLua(response, request, path, s.ssl, s.L);
            } else {
                response.setResponseCode(404);
            }
        }
    } else {
        // If lua file exists
        path += ".lua";
        if (std::filesystem::exists(path)) {
            serveLua(response, request, path, s.ssl, s.L);
        } else {
            response.setResponseCode(404);
        }
    }

    // Convert to char* then send response
    std::string str = response.toString();
    size_t len = str.length();

    for (size_t i = 0; i < len; i += READ_SIZE) {
        size_t end = std::min(i + READ_SIZE, len);
        int l = static_cast<int>(end - i);
        std::string chunk = str.substr(i, l);
        SSL_write(s.ssl, chunk.c_str(), l);
    }

    // Close connection
    SSL_shutdown(s.ssl);
    SSL_free(s.ssl);
    close(s.client);
}

void serveLua(Response &response, Request &request, std::filesystem::path &path, SSL *ssl, lua_State *L) {
    response.setHeader("Content-Type", "text/html");

    int error = luaL_dofile(L, path.c_str());

    if (error) {
        response.setBody("Lua error");
        response.setResponseCode(500);
    } else {
        lua_getglobal(L, "f");

        // Push request to LUA
        lua_newtable(L);
        int top = lua_gettop(L);

        std::map<std::string, std::string> headers = request.getHeaders();
        std::map<std::string, std::string>::iterator it, end;

        for (it = headers.begin(); it != headers.end(); ++it) {
            const char *key = it->first.c_str();
            const char *value = it->second.c_str();
            lua_pushlstring(L, key, it->first.size());
            lua_pushlstring(L, value, it->second.size());
            lua_settable(L, top);
        }

        lua_setglobal(L, "request");

        if (lua_pcall(L, 0, 1, 0) != 0) {
            response.setResponseCode(500);
        }

        if (!lua_isstring(L, -1)) {
            response.setResponseCode(500);
        }
        size_t len;
        const char *res = lua_tolstring(L, -1, &len);
        const std::string str(res);
        lua_pop(L, 1);

        response.setHeader("Content-Length", std::to_string(len));

        response.setBody(str);
    }
}

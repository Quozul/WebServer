#include "servlet.h"

void servelet(SSL* ssl, lua_State * L, struct sockaddr_in& addr) {
    // Read request
    int status;
    unsigned long received = 0;
    std::string requestString;
    char * buffer = new char[READ_SIZE];
    do {
        status = SSL_read(ssl, buffer, READ_SIZE);
        received += status;
        std::string str(buffer);
        requestString += str.substr(0, status);
    } while (status == READ_SIZE);
    delete[] buffer;

    // Parse request
    Request request(requestString);

    Response response;

    // If file exists
    std::filesystem::path path = std::filesystem::current_path();
    path += "/server" + request.getPath();

    if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path)) {
        std::ifstream file(path);
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        response.setBody(contents);
    } else {
        path += ".lua";
        if (std::filesystem::exists(path)) {
            serveLua(response, request, path, ssl, L);
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
        SSL_write(ssl, chunk.c_str(), l);
    }
}

void serveLua(Response &response, Request &request, std::filesystem::path &path, SSL* ssl, lua_State * L) {
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
            const char* key = it->first.c_str();
            const char* value = it->second.c_str();
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
        const char * res = lua_tolstring(L, -1, &len);
        const std::string str(res);
        lua_pop(L, 1);

        response.setHeader("Content-Length", std::to_string(len));

        response.setBody(str);
    }
}
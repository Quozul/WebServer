#include "servlet.hpp"

void servlet(Connection &s) {
    std::string requestString = readString(s);

    // Parse request
    Request request(requestString);
    Response response;
    response.setSSL(s.ssl);

    std::string reqPath = request.getPath();
    std::string path = *s.path;
    path += reqPath;

    // If file exists
    if (std::filesystem::exists(path)) {
        if (std::filesystem::is_regular_file(path)) {
            serveFile(response, path, s);
            return;
        }

        if (std::filesystem::is_directory(path)) {
            if (!path.ends_with('/')) path += '/';
            path += "index";
        }
    }

    // If lua file exists
    std::string filename = ".lua";
    if (std::filesystem::exists(path + filename)) {
        // If lua file exists
        path += filename;
        serveLua(response, request, path, s);
        return;
    }

    filename = ".html";
    if (std::filesystem::exists(path + filename)) {
        // Serve file
        path += filename;
        serveFile(response, path, s);
        return;
    }

    response.setResponseCode(404);

    std::string str = response.toString();
    serveString(s.ssl, str);
}

std::string readString(const Connection &s) {
    // Read request
    int read, pending;
    size_t received = 0;
    char *readBytes = static_cast<char *>(std::malloc(1));
    char *buffer = new char[READ_SIZE];
    do {
        read = SSL_read(s.ssl, buffer, READ_SIZE);
        pending = SSL_pending(s.ssl);

        readBytes = static_cast<char *>(std::realloc(readBytes, received + read + 1));
        std::memcpy(readBytes + received, buffer, read);
        received += read;
    } while (pending > 0 && read > 0);
    delete[] buffer;

    *(readBytes + received) = '\0'; // Add end of string byte

    std::string requestString(readBytes);
    std::free(readBytes);

    return requestString;
}

void serveFile(Response &response, std::string &path, Connection &s) {
    // TODO: Store file in memory for quicker access
    // TODO: Verify method is GET
    // TODO: Add support for "Range" header to send partial files

    // Loop in the file while sending chunks of it
    std::streampos end, cur = 0;
    long len;
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    end = file.tellg();

    const std::string ext = path.substr(path.find_last_of('.') + 1);
    const std::string mime = s.mime_types.at(ext);

    response.setHeader("Content-Length", std::to_string(end));
    response.setHeader("Content-Type", mime);


    // Sends the headers
    response.sendHeaders();

    if (file.is_open()) {
        char *buf = new char[READ_SIZE];

        do {
            len = std::min(static_cast<long>(READ_SIZE), static_cast<long>(end - cur));
            file.seekg(cur);
            file.read(buf, len);
            SSL_write(s.ssl, buf, len);
        } while ((cur += READ_SIZE) < end);

        delete[] buf;
        file.close();
    }
}

void serveLua(Response &response, Request &request, std::string &path, Connection &s) {
    int error = luaL_dofile(s.L, path.c_str());

    if (error) {
        response.setResponseCode(500);
    } else {
        std::string method = request.getMethod();
        for (auto & c: method) c = static_cast<char>(std::toupper(static_cast<int>(c)));
        int type = lua_getglobal(s.L, method.c_str());
        if (type != LUA_TFUNCTION) {
            response.setResponseCode(404);
            response.sendHeaders();
            return;
        }

        // Create the request table
        lua_newtable(s.L);
        int requestLuaTable = lua_gettop(s.L);

        lua_pushstring(s.L, request.getVersion().c_str());
        lua_setfield(s.L, requestLuaTable, "http_version");

        lua_pushstring(s.L, request.getMethod().c_str());
        lua_setfield(s.L, requestLuaTable, "method");

        lua_pushstring(s.L, request.getPath().c_str());
        lua_setfield(s.L, requestLuaTable, "path");

        lua_pushstring(s.L, request.getBody().c_str());
        lua_setfield(s.L, requestLuaTable, "body");

        // Pointer to the request, used for c functions bellow
        lua_pushlightuserdata(s.L, &request);
        lua_setfield(s.L, requestLuaTable, "p");

        lua_pushlightuserdata(s.L, &response);

        // Call the function
        if (lua_pcall(s.L, /* Function argument count */ 2, /* Function return count */ 1, 0) != 0) {
            response.setResponseCode(500);
        }

        // Read the result of the function
        if (!lua_isstring(s.L, -1)) {
            response.setResponseCode(500);
        }
        size_t len;
        const char *res = lua_tolstring(s.L, -1, &len);
        lua_pop(s.L, 1);

        lua_settop(s.L, 0); // Clear lua stack

        if (!response.headers_sent) {
            response.setHeader("Content-Length", std::to_string(len));
            response.sendHeaders();
        }

        // Sends what lua script returned
        serveCharArray(s.ssl, res, len);
    }
}

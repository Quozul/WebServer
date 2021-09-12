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

    std::string path = std::filesystem::path(s.config.at("server"));
    path += reqPath;

    // If file exists
    if (std::filesystem::exists(path)) {
        if (std::filesystem::is_regular_file(path)) {
            serveFile(response, path);
        } else if (std::filesystem::is_directory(path)) {
            std::string filename = "index.lua";
            if (std::filesystem::exists(path + filename)) {
                path += filename;
                // If lua file exists
                serveLua(response, request, path, s);
            } else {
                filename = "index.html";
                if (std::filesystem::exists(path + filename)) {
                    // Serve index/html
                    path += filename;
                    serveFile(response, path);
                } else {
                    response.setResponseCode(404);
                }
            }
        }
    } else {
        // If lua file exists
        path += ".lua";
        if (std::filesystem::exists(path)) {
            serveLua(response, request, path, s);
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

void serveFile(Response &response, std::string &path) {
    // Open file as read only
    std::ifstream file(path);
    std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    response.setHeader("Content-Length", std::to_string(contents.length()));
    response.setBody(contents);
}

void serveLua(Response &response, Request &request, std::string &path, serve s) {
    response.setHeader("Content-Type", "text/html");

    int error = luaL_dofile(s.L, path.c_str());

    if (error) {
        response.setResponseCode(500);
    } else {
        lua_getglobal(s.L, "f");

        // Create the request table
        lua_newtable(s.L);
        int top = lua_gettop(s.L);

        std::map<std::string, std::string> headers = request.getHeaders();
        std::map<std::string, std::string>::iterator it, end;

        for (it = headers.begin(); it != headers.end(); ++it) {
            const char *key = it->first.c_str();
            const char *value = it->second.c_str();
            lua_pushlstring(s.L, key, it->first.size());
            lua_pushlstring(s.L, value, it->second.size());
            lua_settable(s.L, top);
        }

        // Call the function
        if (lua_pcall(s.L, /* Function argument count */ 1, /* Function return count */ 1, 0) != 0) {
            response.setResponseCode(500);
        }

        // Read the result of the function
        if (!lua_isstring(s.L, -1)) {
            response.setResponseCode(500);
        }
        size_t len;
        const char *res = lua_tolstring(s.L, -1, &len);
        lua_pop(s.L, 1);

        response.setHeader("Content-Length", std::to_string(len));
        response.setBody(const_cast<char *>(res));
    }
}

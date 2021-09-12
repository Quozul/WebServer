#include "servlet.h"

void servelet(serve &s) {
    // Read request
    int read, pending;
    size_t received = 0;
    char *readBytes = static_cast<char *>(malloc(1));
    char *buffer = new char[READ_SIZE];
    do {
        read = SSL_read(s.ssl, buffer, READ_SIZE);
        pending = SSL_pending(s.ssl);

        if (read > 0) {
            readBytes = static_cast<char *>(std::realloc(readBytes, received + read + 1));
            std::memcpy(readBytes + received, buffer, read);
            received += read;
        }

        if (pending == 0) break;
    } while (true);
    delete[] buffer;

    *(readBytes + received) = '\0';

    std::string requestString(readBytes);
    // Parse request
    Request request(requestString);
    std::free(readBytes);
    Response response;

    std::string reqPath = request.getPath();
    std::string path = std::filesystem::path(s.config.at("server"));
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
    serveString(s, str);
}

void serveString(serve &s, std::string &str) {
    // Convert to char* then send response
    size_t len = str.length();
    char *buf = new char[READ_SIZE];
    const char *res = str.c_str();

    for (size_t i = 0; i < len; i += READ_SIZE) {
        size_t end = std::min(i + READ_SIZE, len);
        size_t l = end - i;
        std::memcpy(buf, res + i, l);
        SSL_write(s.ssl, buf, l);
    }
}

void serveCharArray(serve &s, const char *res, size_t len) {
    char *buf = new char[READ_SIZE];

    for (size_t i = 0; i < len; i += READ_SIZE) {
        size_t end = std::min(i + READ_SIZE, len);
        size_t l = end - i;
        std::memcpy(buf, res + i, l);
        SSL_write(s.ssl, buf, l);
    }
}

void serveFile(Response &response, std::string &path, serve &s) {
    // Loop in the file while sending chunks of it
    std::streampos begin, end, cur, len;
    std::ifstream file(path, std::ios::binary);
    begin = file.tellg();
    file.seekg(0, std::ios::end);
    end = file.tellg();

    response.setHeader("Content-Length", std::to_string(end - begin));

    // Sends the headers
    std::string headerString = response.getHeadersAsString();
    serveString(s, headerString);

    char *buf = new char[READ_SIZE];
    if (file.is_open()) {
        do {
            len = std::min(static_cast<long>(READ_SIZE), end - cur);
            file.seekg(cur);
            file.read(buf, len);
            SSL_write(s.ssl, buf, len);
        } while ((cur += READ_SIZE) < end);
        file.close();
    }

    delete[] buf;
}

void serveLua(Response &response, Request &request, std::string &path, serve &s) {
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
        lua_settop(s.L, 0);

        response.setHeader("Content-Length", std::to_string(len));

        // Send headers
        std::string headerString = response.getHeadersAsString();
        serveString(s, headerString);

        // Sends what lua script returned
        serveCharArray(s, res, len);
    }
}
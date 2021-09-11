#include "servlet.h"

void servelet(SSL* ssl, lua_State * L, struct sockaddr_in& addr) {
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

    Request request(requestString);


    std::string result;

    Response response;
    response.setHeader("Content-Type", "text/html");

    int error = luaL_dofile(L, "server/index.lua");

    if (error) {
        response.setBody("Lua error");
        response.setResponseCode(500);
    } else {
        lua_getglobal(L, "f");

        if (lua_pcall(L, 0, 1, 0) != 0) {
            response.setResponseCode(500);
        }

        /*size_t len;
        response.setHeader("Content-Length", std::to_string(len));*/

        if (!lua_isstring(L, -1)) {
            response.setResponseCode(500);
        }
        const char * res = lua_tostring(L, -1);
        const std::string str(res);
        lua_pop(L, 1);  /* pop returned value */

        response.setBody(str);
    }

    std::string str = response.toString();
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    SSL_write(ssl, cstr, strlen(cstr));
    delete[] cstr;
}
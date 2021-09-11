#include "Request.h"

Request::Request(const std::string &request) {
    size_t header_end = request.find("\n\n");
    size_t header_start = request.find('\n');

    std::string command = request.substr(0, header_start);
    std::string headers = request.substr(header_start + 1, header_end);

    if (header_end != std::string::npos) body = request.substr(header_end + 1);

    size_t pos, sep;
    unsigned short i = 0;
    std::string token;
    while ((pos = command.find(' ')) != std::string::npos) {
        token = command.substr(0, pos);
        switch (i++) {
            case 0:
                this->method = token;
                break;
            case 1:
                this->path = token;
                break;
            case 2:
                this->http_version = token;
                break;
        }
        command.erase(0, pos + 1);
    }

    while ((pos = headers.find('\n')) != std::string::npos) {
        token = headers.substr(0, pos);
        sep = token.find(':');
        if (sep != std::string::npos) {
            this->headers.insert({token.substr(0, sep), token.substr(sep + 1)});
        }
        headers.erase(0, pos + 1);
    }
}

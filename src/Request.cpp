#include "Request.h"

Request::Request(const std::string &request) {
    size_t header_block_end = request.find(CRLF_CRLF);
    size_t status_line_end = request.find(CRLF);

    std::string status_line = request.substr(0, status_line_end);
    std::string headers = request.substr(status_line_end + CRLF_LEN, header_block_end);

    if (header_block_end != std::string::npos) {
        body = request.substr(header_block_end + CRLF_CRLF_LEN);
    }

    size_t pos, sep;
    unsigned short i = 0;
    std::string token;
    while ((pos = status_line.find(' ')) != std::string::npos) {
        token = status_line.substr(0, pos);
        switch (i++) {
            case 0:
                this->method = token;
                break;
            case 1:
                sep = token.find('?');
                if (sep != std::string::npos) {
                    this->path = token.substr(0, sep);
                    this->raw_params = token.substr(sep + 1);
                } else {
                    this->path = token;
                }

                break;
        }
        status_line.erase(0, pos + 1);
    }

    this->http_version = status_line; // Remaining is the http version

    // Parse parameters
    std::string raw_params = this->raw_params;
    while ((pos = raw_params.find('&'))) {
        token = raw_params.substr(0, pos);
        sep = token.find('=');
        if (sep != std::string::npos) {
            this->params.insert({token.substr(0, sep), token.substr(sep + 1)});
        }
        raw_params.erase(0, pos + 1);
        if (pos == std::string::npos) break;
    }

    // Parse headers
    while ((pos = headers.find(CRLF)) != std::string::npos) {
        token = headers.substr(0, pos);
        sep = token.find(':');
        if (sep != std::string::npos) {
            this->headers.insert({token.substr(0, sep), token.substr(sep + 1)});
        }
        headers.erase(0, pos + CRLF_LEN);
    }
}

#include "Request.hpp"

Request::Request(const std::string &request) {
    size_t header_block_end = request.find(CRLF_CRLF);
    size_t status_line_end = request.find(CRLF);

    if (header_block_end == std::string::npos && status_line_end == std::string::npos) return;

    std::string status_line = request.substr(0, status_line_end);
    this->raw_headers = request.substr(status_line_end + CRLF_LEN, header_block_end);

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

                // TODO: Parse url encoded path
                this->path = decodeURIComponent(this->path);

                // TODO: Prevent directory traversal attack

                break;
        }
        status_line.erase(0, pos + 1);
    }

    this->http_version = status_line; // Remaining is the http version
}

void Request::parseHeaders() {
    if (!this->headers.empty() || this->raw_headers.empty()) return;

    std::string headers = this->raw_headers;
    size_t pos, sep;
    std::string token;

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

void Request::parseParams() {
    if (!this->params.empty() || this->raw_params.empty()) return;

    std::string raw_params = this->raw_params;
    size_t pos, sep;
    std::string token;

    // Parse parameters
    while ((pos = raw_params.find('&'))) {
        token = decodeURIComponent(raw_params.substr(0, pos));
        sep = token.find('=');
        if (sep != std::string::npos) {
            this->params.insert({token.substr(0, sep), token.substr(sep + 1)});
        } else {
            this->params.insert({token, ""});
        }
        raw_params.erase(0, pos + 1);
        if (pos == std::string::npos) break;
    }
}
#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H

#include <string>

#include "../responses/Request.h"


class RequestParser {
    std::string full_request;

public:
    Request request;

    void append_content(const std::string &content);

    [[nodiscard]] size_t remaining_bytes() const;

    [[nodiscard]] bool is_complete() const;
};

void parse_status_line(Request &request, const std::string &status_line);

#endif //REQUESTPARSER_H

#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H

#include <string>

#include "../responses/Request.h"

enum ParsingState {
    Status,
    Headers,
    Body,
};


class RequestParser {
    std::string full_request;
    ParsingState state = Status;
    size_t parsed_bytes = 0;

public:
    Request request;

    void append_content(const std::string &content);

    [[nodiscard]] size_t remaining_bytes() const;

    [[nodiscard]] bool is_complete() const;

    [[nodiscard]] size_t get_content_length() const;

    [[nodiscard]] bool has_body() const;
};

void parse_status_line(Request &request, const std::string &status_line);

#endif //REQUESTPARSER_H

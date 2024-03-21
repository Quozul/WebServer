#include "RequestParser.h"

#include <iostream>
#include <regex>
#include "../string_manipulation.h"
#include "../tracing.h"

#define CRLF "\r\n"
#define CRLF_CRLF "\r\n\r\n"


void RequestParser::append_content(const std::string &content) {
    full_request.append(content);
    bool skip = false;

    if (state == Status) {
        const auto status_line_end_index = content.find(CRLF);
        auto status_line = content.substr(0, status_line_end_index);
        parse_status_line(request, rtrim(status_line));
        parsed_bytes = status_line_end_index + 2;

        state = Headers;
    }

    if (state == Headers) {
        const size_t start_of_body = full_request.find(CRLF_CRLF, parsed_bytes);
        auto headers = full_request.substr(parsed_bytes, start_of_body);

        request.headers = parse_key_values(headers);

        parsed_bytes = start_of_body + 4;

        if (has_body()) {
            skip = true;
            state = Body;
        }
    }

    if (state == Body) {
        const auto body = skip ? content.substr(parsed_bytes) : content;
        request.body.append(body);
    }
}

size_t RequestParser::remaining_bytes() const {
    if (full_request.empty()) {
        return -1; // This causes an overflow telling to read as much as possible
    }

    const size_t content_length = get_content_length();

    if (content_length == 0) {
        return 0;
    }

    const size_t body_size = request.body.size();

    if (body_size >= content_length) {
        return 0;
    }

    const size_t remaining = content_length - body_size;
    constexpr size_t zero = 0;
    const size_t remaining_adjusted = std::max(zero, remaining);

    return remaining_adjusted;
}

bool RequestParser::is_complete() const {
    return remaining_bytes() == 0;
}

size_t RequestParser::get_content_length() const {
    const auto content_length = request.get_header("content-length");
    if (content_length.has_value()) {
        const size_t numerical_value = std::stoi(content_length.value());
        return numerical_value;
    }
    return 0;
}

bool RequestParser::has_body() const {
    return get_content_length() != 0;
}

void parse_status_line(Request &request, const std::string &status_line) {
    const std::regex word_regex("^([^ ]+) ([^ ]+) (HTTP/[0-9.]{1,3})$");

    const auto words_begin = std::sregex_iterator(status_line.begin(), status_line.end(), word_regex);
    const auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        if (const std::smatch &match = *i; match.size() == 4) {
            for (size_t j = 0; j < match.size(); ++j) {
                switch (j) {
                    case 1:
                        request.method = match[j];
                        break;
                    case 2:
                        request.full_url = match[j];
                        break;
                    case 3:
                        request.protocol = match[j];
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

#include "RequestParser.h"

#include <iostream>
#include <regex>
#include "../string_manipulation.h"

#define CRLF "\r\n"
#define CRLF_CRLF "\r\n\r\n"


void RequestParser::append_content(const std::string &content) {
    full_request.append(content);

    // TODO: Resume parsing where we left of to skip a few steps

    const auto status_line_end_index = full_request.find(CRLF);
    auto status_line = full_request.substr(0, status_line_end_index);

    parse_status_line(request, rtrim(status_line));

    const size_t start_of_body = full_request.find(CRLF_CRLF, status_line_end_index);
    auto headers = full_request.substr(status_line_end_index, start_of_body);

    request.headers = parse_key_value(headers);

    request.body = full_request.substr(start_of_body + 4);
}

size_t RequestParser::remaining_bytes() const {
    if (full_request.empty()) {
        return -1; // This causes an overflow telling to read as much as possible
    }

    if (const auto content_length = request.get_header("content-length"); content_length.has_value()) {
        const size_t numerical_value = std::stoi(content_length.value());
        const size_t body_size = request.body.size();

        if (body_size >= numerical_value) {
            return 0;
        }

        const size_t remaining = numerical_value - body_size;
        constexpr size_t zero = 0;
        const size_t remaining_adjusted = std::max(zero, remaining);
        return remaining_adjusted;
    }

    return 0;
}

bool RequestParser::is_complete() const {
    return remaining_bytes() == 0;
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

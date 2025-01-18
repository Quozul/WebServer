#include "RequestParser.h"

#include "../string_manipulation.h"

#include <regex>
#include <spdlog/spdlog.h>

#define CRLF "\r\n"
#define CRLF_CRLF "\r\n\r\n"

void RequestParser::append_content(const std::string &content) {
    if (content.empty()) {
        return;
    }

    has_already_appended_content = true;
    bool skip = false;

    if (state == Status) {
        const auto status_line_end_index = content.find(CRLF);
        auto status_line = content.substr(0, status_line_end_index);
        parse_status_line(request, rtrim(status_line));
        parsed_bytes = status_line_end_index + 2;

        state = Headers;
    }

    if (state == Headers) {
        const size_t start_of_body = content.find(CRLF_CRLF, parsed_bytes);
        auto headers = content.substr(parsed_bytes, start_of_body);

        request.headers = parse_key_values(headers);
        parsed_bytes = start_of_body + 4;
        expected_body_size = get_content_length();

        if (has_body()) {
            request.body.reserve(expected_body_size);
            skip = true;
            state = Body;
        }
    }

    if (state == Body) {
        const auto body = skip ? content.substr(parsed_bytes) : content;
        request.body.append(body.c_str(), body.size());
    }
}

size_t RequestParser::remaining_bytes() const {
    if (!has_already_appended_content) {
        return -1; // This causes an overflow telling to read as much as possible
    }

    if (expected_body_size == 0) {
        return 0;
    }

    const size_t body_size = request.body.size();

    if (body_size >= expected_body_size) {
        return 0;
    }

    const size_t remaining = expected_body_size - body_size;
    constexpr size_t zero = 0;
    const size_t remaining_adjusted = std::max(zero, remaining);

    return remaining_adjusted;
}

bool RequestParser::has_more() const { return remaining_bytes() != 0; }

size_t RequestParser::get_content_length() const {
    const auto content_length = request.get_header("content-length");
    if (content_length.has_value()) {
        const size_t numerical_value = std::stoi(content_length.value());
        return numerical_value;
    }
    return 0;
}

bool RequestParser::has_body() const { return expected_body_size != 0; }

bool RequestParser::is_keep_alive() const {
    const auto connection = request.get_header("connection");
    return connection == "keep-alive";
}

ParsingState RequestParser::get_state() const {
    return state;
}

void parse_status_line(Request &request, const std::string &status_line) {
    const size_t method_end = status_line.find(' ');
    const size_t protocol_start = status_line.rfind(' ');

    if (method_end == protocol_start) {
        request.url = Url::empty();
    } else {
        request.url = Url::parse(status_line.substr(method_end + 1, protocol_start - method_end - 1));
    }

    request.method = status_line.substr(0, method_end);
    request.protocol = status_line.substr(protocol_start + 1);
}

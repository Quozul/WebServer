#include "string_manipulation.h"

#include <algorithm>
#include <iostream>
#include <spdlog/fmt/fmt.h>
#include <sstream>

std::string &ltrim(std::string &value) { return value.erase(0, value.find_first_not_of(" \t\r\n")); }

std::string &rtrim(std::string &value) { return value.erase(value.find_last_not_of(" \t\r\n") + 1); }

std::string &trim(std::string &s) {
    rtrim(s);
    ltrim(s);
    return s;
}

void to_lower_case_in_place(std::string &string) {
    std::transform(string.begin(), string.end(), string.begin(),
                   [](const unsigned char character) { return std::tolower(character); });
}

std::unordered_map<std::string, std::string> parse_key_values(const std::string &raw_lines) {
    std::unordered_map<std::string, std::string> headers;
    const char *start = raw_lines.c_str();
    const char *end = start + raw_lines.length();
    const char *line_start = start;

    while (line_start < end) {
        const char *line_end = std::find(line_start, end, '\n');

        const char *equal_pos;
        if (*line_start == '#' || (equal_pos = std::find(line_start, line_end, ':')) == line_end) {
            line_start = line_end + 1;
            continue;
        }

        const size_t key_len = equal_pos - line_start;
        size_t value_len = line_end - (equal_pos + 1);

        const char *comment_start = std::find(equal_pos + 1, line_end, '#');
        if (comment_start != line_end) {
            value_len = comment_start - (equal_pos + 1);
        }

        std::string key(line_start, key_len);
        std::string value(equal_pos + 1, value_len);

        trim(key);
        trim(value);
        to_lower_case_in_place(key);

        headers.emplace(key, value);

        line_start = line_end + 1;
    }

    return headers;
}

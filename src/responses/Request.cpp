#include "Request.h"
#include "../string_manipulation.h"

std::optional<std::string> Request::get_header(const std::string &key) const {
    std::string key_copy = key;
    to_lower_case_in_place(key_copy);

    if (headers.contains(key_copy)) {
        return std::optional{headers.at(key_copy)};
    }

    return std::nullopt;
}

#ifndef STRING_MANIPULATION_H
#define STRING_MANIPULATION_H

#include <string>
#include <unordered_map>

std::string &ltrim(std::string &value);

std::string &rtrim(std::string &value);

std::string &trim(std::string &s);

void to_lower_case_in_place(std::string &string);

std::unordered_map<std::string, std::string> parse_key_values(const std::string &raw_lines);

#endif

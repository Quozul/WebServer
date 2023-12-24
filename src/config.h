#ifndef CONFIG_H
#define CONFIG_H

#include <optional>
#include <string>
#include <filesystem>

struct Config {
    std::optional<std::string> cert;
    std::optional<std::string> key;
    int port{};
};

std::optional<std::filesystem::path> look_for_config();

Config read_config();

#endif

#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>
#include <optional>

struct Config {
    std::optional<std::filesystem::path> cert;
    std::optional<std::filesystem::path> key;
    int port = 8080;
};

std::optional<std::filesystem::path> look_for_config();

Config read_config();

#endif

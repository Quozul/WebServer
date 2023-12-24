#ifndef CONFIG_H
#define CONFIG_H

#include <optional>
#include <filesystem>

struct Config {
    std::optional<std::filesystem::path> cert;
    std::optional<std::filesystem::path> key;
    int port{};
};

std::optional<std::filesystem::path> look_for_config();

Config read_config();

#endif

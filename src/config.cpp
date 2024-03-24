#include "config.h"

#include <fstream>

#include "request/Request.h"
#include "string_manipulation.h"

std::optional<std::filesystem::path> look_for_config() {
    const auto current_path = std::filesystem::current_path();

    if (const auto config_path = current_path / "config"; exists(config_path)) {
        return std::optional{config_path};
    }

    return std::nullopt;
}

Config read_config() {
    Config config;

    if (const auto config_path = look_for_config(); config_path.has_value()) {
        std::ifstream config_file(config_path.value());
        auto content = std::string(std::istreambuf_iterator(config_file), std::istreambuf_iterator<char>());
        auto parsed_config = parse_key_values(content);

        if (parsed_config.contains("cert")) {
            config.cert = parsed_config["cert"];
        }

        if (parsed_config.contains("key")) {
            config.key = parsed_config["key"];
        }

        if (parsed_config.contains("port")) {
            config.port = std::stoi(parsed_config["port"]);
        }

        if (parsed_config.contains("access_logs")) {
            config.access_logs = parsed_config["access_logs"] == "true";
        }
    }

    return config;
}

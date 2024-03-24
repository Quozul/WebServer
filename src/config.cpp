#include "config.h"

#include "string_manipulation.h"

#include <fstream>
#include <spdlog/spdlog.h>

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

        if (parsed_config.contains("log_level")) {
            const auto log_level = parsed_config["log_level"];

            if (log_level == "trace") {
                spdlog::set_level(spdlog::level::level_enum::trace);
                spdlog::trace("Logging level is set to 'trace'");
            } else if (log_level == "debug") {
                spdlog::set_level(spdlog::level::level_enum::debug);
                spdlog::debug("Logging level is set to 'debug'");
            } else if (log_level == "warn") {
                spdlog::set_level(spdlog::level::level_enum::warn);
                spdlog::warn("Logging level is set to 'warn'");
            } else if (log_level == "err") {
                spdlog::set_level(spdlog::level::level_enum::err);
                spdlog::error("Logging level is set to 'err'");
            } else if (log_level == "critical") {
                spdlog::set_level(spdlog::level::level_enum::critical);
                spdlog::critical("Logging level is set to 'critical'");
            } else if (log_level == "off") {
                spdlog::set_level(spdlog::level::level_enum::off);
            } else {
                spdlog::set_level(spdlog::level::level_enum::info);
                spdlog::info("Logging level is set to 'info'");
            }
        }
    }

    return config;
}

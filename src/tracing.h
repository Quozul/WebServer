#ifndef TRACING_H
#define TRACING_H

#include <fmt/core.h>
#include <cerrno>
#include <iostream>
#include <chrono>

namespace tracing {
    inline std::string get_formatted_time() {
        const auto now = std::chrono::system_clock::now();
        const auto now_c = std::chrono::system_clock::to_time_t(now);
        const auto now_tm = std::localtime(&now_c);
        const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now_tm);

        const auto ms_str = std::to_string(now_ms.count());
        const auto now_str = std::string(buffer) + "." + std::string(3 - ms_str.size(), '0') + ms_str;

        return now_str;
    }

    template<typename... T>
    void trace(const std::string &level, fmt::format_string<T...> fmt, T&&... args) {
        const std::string formatted_string = fmt::format(fmt, std::forward<T>(args)...);
        const std::string output = fmt::format("{}: {} - {}\n", level, get_formatted_time(), formatted_string);
        std::cout << output;
    }

    template<typename... T>
    void info(fmt::format_string<T...> fmt, T&&... args) {
        trace("INFO", fmt, std::forward<T>(args)...);
    }

    template<typename... T>
    void warn(fmt::format_string<T...> fmt, T&&... args) {
        trace("WARN", fmt, std::forward<T>(args)...);
    }

    template<typename... T>
    void error(fmt::format_string<T...> fmt, T&&... args) {
        trace("ERROR", fmt, std::forward<T>(args)...);
    }
}

#endif

#ifndef TRACING_H
#define TRACING_H

#include <format>
#include <cerrno>

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

    template<typename... Args>
    void trace(std::format_string<Args...> fmt, Args &&... args) {
        const std::string formatted_string = std::vformat(fmt.get(), std::make_format_args(args...));
        const std::string output = std::format("INFO: {} - {}\n", get_formatted_time(), formatted_string);

        std::cout << output;
    }

    template<typename... Args>
    void warn(std::format_string<Args...> fmt, Args &&... args) {
        const std::string formatted_string = std::vformat(fmt.get(), std::make_format_args(args...));
        const std::string output = std::format("WARN: {} - {}\n", get_formatted_time(), formatted_string);

        std::cout << output;
    }

    template<typename... Args>
    void error(std::format_string<Args...> fmt, Args &&... args) {
        const std::string formatted_string = std::vformat(fmt.get(), std::make_format_args(args...));
        const std::string output = std::format("WARN: {} - {} ({})\n", get_formatted_time(), formatted_string, strerror(errno));

        std::cerr << output;
    }
}

#endif

#include "Router.h"

#include <spdlog/spdlog.h>

Router &Router::route(const std::string &path, const Handler &handler) {
    routes[path] = handler;
    return *this;
}

Router &Router::not_found(const Handler &handler) {
    not_found_handler = handler;
    return *this;
}

void Router::handle_request(const Request &request, Response &response) const {
    if (const auto path = request.get_url().get_path(); routes.contains(path)) {
        routes.at(path)(request, response);
    } else {
        response.set_status_code(404);
        if (not_found_handler.has_value()) {
            not_found_handler.value()(request, response);
        }
    }

    spdlog::info("\"{} {}\" {}", request.get_method(), request.get_url().get_full_url(), response.get_status_message());
}

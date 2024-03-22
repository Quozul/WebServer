#include "Router.h"

Router &Router::route(const std::string &path, const Handler &handler) {
    this->routes[path] = handler;
    return *this;
}

Router &Router::not_found(const Handler &handler) {
    this->not_found_handler = handler;
    return *this;
}

void Router::handle_request(const Request &request, Response &response) const {
    const auto path = request.get_url().get_path();

    if (!this->routes.contains(path)) {
        response.set_status_code(404);

        if (this->not_found_handler.has_value()) {
            this->not_found_handler.value()(request, response);
        }
    } else {
        this->routes.at(path)(request, response);
    }
}

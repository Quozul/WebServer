#ifndef ROUTER_H
#define ROUTER_H

#include <functional>
#include <map>

#include "request/Request.h"
#include "response/Response.h"

typedef std::function<void(const Request &, Response &)> Handler;

class Router {
    std::map<std::string, Handler> routes;
    std::optional<Handler> not_found_handler;

  public:
    Router &route(const std::string &path, const Handler &handler);

    Router &not_found(const Handler &handler);

    void handle_request(const Request &request, Response &response) const;
};

#endif

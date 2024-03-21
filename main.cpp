#include <iostream>
#include <csignal>
#include <csignal>
#include <fmt/core.h>

#include "src/App.h"
#include "src/config.h"
#include "src/responses/Request.h"
#include "src/responses/Response.h"

App *g_app;

void signal_handler(int) {
    if (g_app) {
        g_app->close_socket();
        std::exit(0);
    }
}

int main() {
    App app;
    g_app = &app;
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGABRT, signal_handler);

    std::signal(SIGPIPE, SIG_IGN); // Disable SIGPIPE

    const auto [cert, key, port] = read_config();

    app.route("/", []([[maybe_unused]] const Request &request) {
        Response response;

        response.set_header("content-type", "text/html");
        response.set_body(
            "This is a custom web server developed in C++. Try the other route <a href='/hello'>/hello</a>.");

        return response;
    });

    app.route("/hello", [](const Request &request) {
        Response response;
        response.set_header("content-type", "text/html");

        if (const auto name = request.get_param("name"); name.has_value()) {
            response.set_body("<h1>Hello, " + name.value() + "!</h1>");
        } else {
            response.set_body("<a href='/hello?name=world'>World</a>");
        }

        return response;
    });

    app.route("/file", [](const Request &request) {
        Response response;
        response.set_header("content-type", "text/html");

        if (request.get_method() != "POST") {
            response.set_body(R"(
                <form action="/file" method="post" enctype="multipart/form-data">
                    <input type="file" name="file" />
                    <input type="submit" value="Upload" />
                </form>
            )");
        } else {
            const auto body = request.get_body();
            response.set_body(fmt::format("File size: {}", body.size()));
        }

        return response;
    });

    if (cert.has_value() && key.has_value()) {
        app.enable_ssl(cert.value(), key.value());
    }

    app.run(port);

    return 0;
}

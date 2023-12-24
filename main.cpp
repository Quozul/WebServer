#include <iostream>
#include <csignal>
#include <csignal>

#include "src/App.h"
#include "src/config.h"
#include "src/responses/Request.h"
#include "src/responses/Response.h"

int main() {
    App app;
    std::signal(SIGPIPE, SIG_IGN); // Disable SIGPIPE

    const auto [cert, key, port] = read_config();

    app.route("/", []([[maybe_unused]] const Request &request) {
        Response response;

        response.set_header("content-type", "text/html");
        response.set_body("This is a custom web server developed in C++ with the goal to use it on a RISC-V board. Try the other route <a href='/hello'>/hello</a>.");

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

    if (cert.has_value() && key.has_value()) {
        app
                .enable_ssl(cert.value(), key.value())
                .run(port);
    }

    app.run(port);

    return 0;
}

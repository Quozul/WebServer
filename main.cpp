#include <iostream>
#include <csignal>
#include <csignal>
#include <sstream>

#include "src/App.h"
#include "src/responses/Request.h"
#include "src/responses/Response.h"

int main() {
    App app;
    std::signal(SIGPIPE, SIG_IGN); // Disable SIGPIPE

    app.route("/", [](const Request &request) {
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

    const auto cert_file = "cert.pem";
    const auto key_file = "key.pem";

    app.enable_ssl(cert_file, key_file).run(8080);

    return 0;
}

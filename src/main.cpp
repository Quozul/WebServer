#include <iostream>
#include <csignal>
#include <csignal>
#include <sstream>

#include "App.h"
#include "responses/Request.h"
#include "responses/Response.h"

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

        try {
            const std::string name = request.get_param("name");
            response.set_body("<h1>Hello, " + name + "!</h1>");
        } catch (const std::out_of_range &e) {
            response.set_body("<a href='/hello?name=world'>World</a>");
        }

        return response;
    });

    const auto cert_file = "cert.pem";
    const auto key_file = "key.pem";

    app.enable_ssl(cert_file, key_file).run(8080);

    return 0;
}

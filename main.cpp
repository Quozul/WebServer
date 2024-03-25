#include "src/App.h"
#include "src/config.h"
#include "src/request/Request.h"
#include "src/response/Response.h"

#include <csignal>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

App *g_app;

void signal_handler(int) {
    if (g_app) {
        g_app->close_socket();
        std::exit(0);
    }
}

void index_handler([[maybe_unused]] const Request &request, Response &response) {
    response.set_header("content-type", "text/html");
    response.set_body(R"(
        This is a custom web server developed in C++.
        Try the other route <a href='/hello'>/hello</a> or <a href='/file'>file upload</a>.
    )");
}

void not_found_handler(const Request &request, Response &response) {
    response.set_header("content-type", "text/html");
    response.set_body(fmt::format(
        R"(
            The page "{}" you have requestes does not exists.
            <a href='/'>Go to home</a>.
        )",
        request.url.get_path()));
}

void hello_handler(const Request &request, Response &response) {
    response.set_header("content-type", "text/html");

    if (const auto name = request.get_url().get_param("name"); name.has_value()) {
        response.set_body("<h1>Hello, " + name.value() + "!</h1>");
    } else {
        response.set_body("<a href='/hello?name=world'>World</a>");
    }
}

void file_handler(const Request &request, Response &response) {
    response.set_header("content-type", "text/html");

    std::string response_body = R"(
        <form action="/file" method="post" enctype="multipart/form-data">
            <input type="file" name="file" multiple />
            <input type="submit" value="Upload" />
        </form>
        The file(s) are not saved. Try <a href='/image'>image upload</a> as well.
    )";

    if (request.get_method() == "POST") {
        const auto &body = request.get_body();
        response_body.append(fmt::format("Upload size: {} bytes", body.size()));
    }

    response.set_body(response_body);
}

void image_handler(const Request &request, Response &response) {
    if (request.get_method() != "POST") {
        response.set_header("content-type", "text/html");
        response.set_body(R"(
            <form action="/image" method="post" enctype="multipart/form-data">
                <input type="file" name="file" accept="image/jpg" />
                <input type="submit" value="Upload" />
            </form>
            The file(s) are not saved. Try <a href='/file'>file upload</a> as well.
        )");
    } else {
        const auto content = request.get_body().str();
        const size_t pos = content.find("image/jpeg", 0) + 14;

        if (pos != std::string::npos) {
            const auto sub = content.substr(pos);
            response.set_header("content-type", "image/jpg");
            response.set_body(sub);
        } else {
            response.set_status_code(406);
        }
    }
}

int main() {
    const auto router = Router()
                            .route("/", index_handler)
                            .route("/hello", hello_handler)
                            .route("/file", file_handler)
                            .route("/image", image_handler)
                            .not_found(not_found_handler);

    App app(router);
    g_app = &app;
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGPIPE, SIG_IGN); // Disable SIGPIPE

    const auto [cert, key, port] = read_config();

    if (cert.has_value() && key.has_value()) {
        app.enable_ssl(cert.value(), key.value());
    }

    app.run(port);

    return 0;
}

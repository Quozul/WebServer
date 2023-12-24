#include <iostream>
#include <signal.h>
#include <csignal>
#include <bits/signum-generic.h>

#include "App.h"

int main()
{
    const auto app = new App();
    std::signal(SIGPIPE, SIG_IGN); // Disable SIGPIPE

    app->run(8080);
    delete app;

    std::cout << "Hello, World!" << std::endl;
    return 0;
}

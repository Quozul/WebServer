#include "SelectEventLoop.h"

#include <spdlog/spdlog.h>

void SelectEventLoop::add_fd(const int fd) {
    FD_SET(fd, &master_fds);

    if (fd > max_fd) {
        max_fd = fd;
    }
}

void SelectEventLoop::remove_fd(const int fd) { FD_CLR(fd, &master_fds); }

std::set<int> SelectEventLoop::wait_for_events() {
    read_fds = master_fds;
    const int activity = select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr);
    if (activity < 0) {
        const auto error_message = std::strerror(errno);
        spdlog::critical("select error: ({}) '{}'", activity, error_message);
        exit(EXIT_FAILURE);
    }

    std::set<int> ready_fds;
    for (int i = 0; i <= max_fd; ++i) {
        if (FD_ISSET(i, &read_fds)) {
            ready_fds.insert(i);
        }
    }
    return ready_fds;
}

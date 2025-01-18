#include "EpollEventLoop.h"

#include <iostream>
#include <spdlog/spdlog.h>
#include <sys/epoll.h>
#include <unistd.h>

EpollEventLoop::EpollEventLoop() {
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        const auto error_message = std::strerror(errno);
        spdlog::critical("epoll_create1 error: '{}'", error_message);
        exit(EXIT_FAILURE);
    }
}

EpollEventLoop::~EpollEventLoop() { close(epoll_fd); }

void EpollEventLoop::add_fd(const int fd) {
    epoll_event event{};
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLONESHOT;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        const auto error_message = std::strerror(errno);
        spdlog::critical("epoll_ctl add error: '{}'", error_message);
        exit(EXIT_FAILURE);
    }
}

void EpollEventLoop::remove_fd(const int fd) {
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        const auto error_message = std::strerror(errno);
        spdlog::critical("epoll_ctl remove error: '{}'", error_message);
        exit(EXIT_FAILURE);
    }
}

int EpollEventLoop::wait_for_events(epoll_event *events, const int max_events) {
    const int num_events = epoll_wait(epoll_fd, events, max_events, -1);

    if (num_events == -1) {
        const auto error_message = std::strerror(errno);
        spdlog::error("epoll_wait error: '{}'", error_message);
        return -1;
    }

    return num_events;
}

void EpollEventLoop::modify_fd(const int fd) {
    epoll_event event{};
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLONESHOT;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event) == -1) {
        perror("epoll_ctl: modify");
        exit(EXIT_FAILURE);
    }
}

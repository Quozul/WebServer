#ifndef EPOLLEVENTLOOP_H
#define EPOLLEVENTLOOP_H

#include "EventLoop.h"

#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

class EpollEventLoop final : public EventLoop {
    static constexpr int MAX_EVENTS = 256;
    int epoll_fd;

  public:
    EpollEventLoop();

    ~EpollEventLoop() override;

    void add_fd(int fd) override;

    void remove_fd(int fd) override;

    std::set<int> wait_for_events() override;
};

#endif

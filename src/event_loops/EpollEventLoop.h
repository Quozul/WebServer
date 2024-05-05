#ifndef EPOLLEVENTLOOP_H
#define EPOLLEVENTLOOP_H

#include "EventLoop.h"

#include <mutex>
#include <sys/epoll.h>

class EpollEventLoop final : public EventLoop {
    int epoll_fd;

  public:
    EpollEventLoop();

    ~EpollEventLoop() override;

    void add_fd(int fd) override;

    void remove_fd(int fd) override;

    int wait_for_events(epoll_event *events, int max_events) override;

    void modify_fd(int fd) override;
};

#endif

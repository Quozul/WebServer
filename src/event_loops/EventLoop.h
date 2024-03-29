#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <set>
#include <sys/epoll.h>

class EventLoop {
  public:
    virtual ~EventLoop() = default;

    virtual void add_fd(int fd) = 0;

    virtual void remove_fd(int fd) = 0;

    virtual int wait_for_events(epoll_event *events, int max_events) = 0;

    virtual void modify_fd(int fd) = 0;
};

#endif

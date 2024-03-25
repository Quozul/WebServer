#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <set>

class EventLoop {
  public:
    virtual ~EventLoop() = default;

    virtual void add_fd(int fd) = 0;

    virtual void remove_fd(int fd) = 0;

    virtual std::set<int> wait_for_events() = 0;
};

#endif

#ifndef SELECTEVENTLOOP_H
#define SELECTEVENTLOOP_H

#include "EventLoop.h"

#include <iostream>

class SelectEventLoop final : public EventLoop {
    fd_set master_fds{}, read_fds{};
    int max_fd = 0;

  public:
    void add_fd(int fd) override;

    void remove_fd(int fd) override;

    std::set<int> wait_for_events() override;
};

#endif

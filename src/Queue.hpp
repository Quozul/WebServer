// Source: https://juanchopanzacpp.wordpress.com/2013/02/26/concurrent-queue-c11/

#ifndef WEBSERVER_QUEUE_HPP
#define WEBSERVER_QUEUE_HPP

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template<typename T>
class Queue {
public:
    T pop() {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty()) {
            cond_.wait(mlock);
        }
        auto item = queue_.front();
        queue_.pop();
        return item;
    };

    void push(const T &item) {
        queue_.push(item);
        cond_.notify_one();
    };

    int size() {
        return queue_.size();
    };

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

#endif //WEBSERVER_QUEUE_HPP

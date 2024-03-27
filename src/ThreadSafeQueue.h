#ifndef QUEUE_H
#define QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T> class ThreadSafeQueue {
  public:
    T pop() {
        std::unique_lock mlock(mutex_);
        while (queue_.empty()) {
            cond_.wait(mlock);
        }
        auto item = queue_.front();
        queue_.pop();
        return item;
    }

    void push(const T &item) {
        queue_.push(item);
        cond_.notify_one();
    }

    int size() { return queue_.size(); };

  private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

#endif

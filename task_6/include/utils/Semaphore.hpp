#include <mutex>
#include <condition_variable>
#include <chrono>

class Semaphore {
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int counter_;

public:
    explicit Semaphore(int counter = 0) : counter_(counter) {}

    void acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return counter_ > 0; });
        --counter_;
    }

    void release() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++counter_;
        cv_.notify_one();
    }

    bool try_acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (counter_ > 0) {
            --counter_;
            return true;
        }
        return false;
    }

    template<typename Rep, typename Period>
    bool try_acquire_for(const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!cv_.wait_for(lock, timeout, [this]() { return counter_ > 0; }))
            return false;
        --counter_;
        return true;
    }
};

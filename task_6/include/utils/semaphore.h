#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstddef>

namespace std {

template<std::ptrdiff_t MaxValue = 0>
class counting_semaphore {
    static_assert(MaxValue >= 0, "MaxValue must be non-negative");

    std::mutex mtx;
    std::condition_variable cv;
    std::ptrdiff_t count;

public:
    // Конструктор по умолчанию = 0
    counting_semaphore() : count(0) {}

    // Конструктор с начальным значением
    explicit counting_semaphore(std::ptrdiff_t initial_count) : count(initial_count) {}

    void acquire() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]{ return count > 0; });
        --count;
    }

    bool try_acquire() {
        std::unique_lock<std::mutex> lock(mtx);
        if (count > 0) {
            --count;
            return true;
        }
        return false;
    }

    template<class Rep, class Period>
    bool try_acquire_for(const std::chrono::duration<Rep, Period>& rel_time) {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, rel_time, [this]{ return count > 0; })) {
            --count;
            return true;
        }
        return false; // таймаут
    }

    void release() {
        release(1);
    }

    void release(std::ptrdiff_t n) {
        std::unique_lock<std::mutex> lock(mtx);
        count += n;
        if (MaxValue > 0 && count > MaxValue) count = MaxValue;
        for (std::ptrdiff_t i = 0; i < n; ++i)
            cv.notify_one();
    }
};

} // namespace std

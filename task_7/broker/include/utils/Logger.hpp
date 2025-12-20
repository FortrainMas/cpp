#pragma once

#include <atomic>
#include <condition_variable>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>

class Logger {
public:
    static void init() {
        running_.store(true);
        worker_ = std::thread(&Logger::workerLoop);
    }

    static void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            running_.store(false);
        }
        cv_.notify_one();
        if (worker_.joinable())
            worker_.join();
    }

    static void log(const std::string& msg) {
        std::ostringstream oss;

        auto now = std::chrono::system_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&tt);

        oss << "[" << std::put_time(&tm, "%F %T") << "]";
        oss << " [tid=" << std::this_thread::get_id() << "] ";
        oss << msg;

        {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push(oss.str());
        }
        cv_.notify_one();
    }

private:
    static void workerLoop() {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [] {
                return !queue_.empty() || !running_.load();
            });

            while (!queue_.empty()) {
                std::cout << queue_.front() << std::endl;
                queue_.pop();
            }

            if (!running_.load())
                break;
        }
    }

private:
    inline static std::atomic_bool running_{false};
    inline static std::mutex mtx_;
    inline static std::condition_variable cv_;
    inline static std::queue<std::string> queue_;
    inline static std::thread worker_;
};

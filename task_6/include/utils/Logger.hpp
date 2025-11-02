#pragma once

#include <mutex>
#include <iostream>

class Logger {
    private:
        static std::mutex mutex_;
    public:
        static void log(const std::string& message) {
            std::lock_guard<std::mutex> lock(mutex_);
            std::cout << message << std::endl;
        }    
};

inline std::mutex Logger::mutex_;

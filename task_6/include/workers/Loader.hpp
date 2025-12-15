#pragma once

#include <memory>
#include <atomic>
#include <thread>
#include <chrono>

#include "accounting_system/AccountingSystem.hpp"
#include "warehouse/Warehouse.hpp"

class Loader {
private:
    const int work_time_;
    std::atomic_bool running_{false};
    std::thread thread_;
    std::weak_ptr<AccountingSystem> acc_sys_;
    std::weak_ptr<Warehouse> warehouse_;

    void run() {
        while (running_) {
            auto acc = acc_sys_.lock();
            if (!acc) break;
            
            auto task = acc->getNextReceivingDockTask();
            if (task) {
                do_task(*task);
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }


public:
    explicit Loader(std::shared_ptr<AccountingSystem> acc_sys, 
                    std::shared_ptr<Warehouse> warehouse,
                    int work_time = 5)
        : work_time_(work_time)
        , acc_sys_(acc_sys)
        , warehouse_(warehouse) {}

    void start() {
        if (!running_) {
            running_ = true;
            thread_ = std::thread(&Loader::run, this);
        }
    }

    void stop() {
        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    ~Loader() {
        stop();
    }
};

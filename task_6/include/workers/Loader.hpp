#pragma once

#include <memory>
#include <atomic>
#include <thread>
#include <chrono>

#include "accounting_system/AccountingSystem.hpp"
#include "warehouse/Warehouse.hpp"
#include "accounting_system/tasks/TaskDistributionSystem.hpp"
#include "utils/Logger.hpp"

class Loader {
private:
    const int work_time_;
    std::weak_ptr<AccountingSystem> acc_sys_;
    std::weak_ptr<Warehouse> warehouse_;
    std::atomic_bool running_{false};
    std::thread thread_;
    std::weak_ptr<TaskDistributionSystem> task_system_;

    void run() {
        while (running_) {
            auto system = task_system_.lock();
            if (!system) break;

            auto fut = system->getTask();
            auto task = fut.get();
            if (task) {
                Logger::log("Trynna work");
                task->doTask(work_time_, acc_sys_, warehouse_);
            } else {
                Logger::log("Sleeping. Zzzz...");
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

public:
    explicit Loader(int work_time, std::weak_ptr<AccountingSystem> acc_sys, std::weak_ptr<Warehouse> warehouse)
        : task_system_(acc_sys.lock()->getTaskDistributionSystem().lock()), 
            acc_sys_(acc_sys), warehouse_(warehouse), work_time_(work_time) {}

    void start() {
        if (!running_) {
            running_ = true;
            thread_ = std::thread(&Loader::run, this);
        }
    }

    void stop() {
        running_ = false;
        if (thread_.joinable()) thread_.join();
    }

    ~Loader() {
        stop();
    }
};

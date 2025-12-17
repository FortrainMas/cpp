#include <mutex>
#include <thread>
#include <future>
#include <queue>

#include "accounting_system/tasks/Task.hpp"

class TaskDistributionSystem {
private:
    std::mutex mutex_;
    std::queue<std::shared_ptr<Task>> available_tasks;
    std::queue<std::promise<std::shared_ptr<Task>>> waiting_promises;

public:
    void addTask(std::shared_ptr<Task> task) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!waiting_promises.empty()) {
            auto prom = std::move(waiting_promises.front());
            waiting_promises.pop();
            prom.set_value(task); 
        } else {
            available_tasks.push(task);
        }
    }

    std::future<std::shared_ptr<Task>> getTask() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!available_tasks.empty()) {
            auto task = available_tasks.front();
            available_tasks.pop();
            std::promise<std::shared_ptr<Task>> prom;
            auto fut = prom.get_future();
            prom.set_value(task);
            return fut;
        } else {
            std::promise<std::shared_ptr<Task>> prom;
            auto fut = prom.get_future();
            waiting_promises.push(std::move(prom));
            return fut;
        }
    }
};

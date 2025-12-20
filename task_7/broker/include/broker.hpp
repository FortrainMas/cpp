#pragma once

#include <memory>
#include <thread>
#include <chrono>
#include <atomic>

#include "queue.hpp"
#include "utils/BrokerConfig.hpp"


class Broker {
private:
    std::vector<std::shared_ptr<Queue>> queues;
    std::thread thread_;
    std::atomic<bool> running_{false};
    
    void worker() {
        Logger::log("Started broker");
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            for (auto& queue : queues) {
                queue->pruneQueue();
            }
        }
    }
    
public:
    Broker(std::vector<std::shared_ptr<Queue>> queues) : queues(std::move(queues)) {}
    Broker(BrokerConfig brokerConfig) {
        for (auto& queueConfig : brokerConfig.queues) {
            queues.push_back(std::make_shared<Queue>(
                queueConfig.name,
                queueConfig.type,
                queueConfig.durability,
                queueConfig.ttl));
        }
    }

    std::shared_ptr<Queue> getQueue(std::string name) {
        for (auto& queue : queues) {
            if (queue->getName() == name) {
                return queue;
            }
        }
        
        return nullptr;
    }
    
    ~Broker() {
        stop();
    }
    
    void start() {
        if (running_) {
            return;
        }
        
        running_ = true;
        thread_ = std::thread(&Broker::worker, this);
    }
    
    void stop() {
        if (!running_) {
            return;
        }
        
        running_ = false;
        
        if (thread_.joinable()) {
            thread_.join();
        }
    }
};

#pragma once

#include <memory>
#include <atomic>
#include <thread>
#include <chrono>

#include "accounting_system/AccountingSystem.hpp"
#include "warehouse/Warehouse.hpp"
#include "accounting_system/tasks/TaskDistributionSystem.hpp"
#include "accounting_system/tasks/UnloadTask.hpp"
#include "accounting_system/tasks/PackingTask.hpp"
#include "utils/Logger.hpp"
#include "utils/RandomGenerator.hpp"



class Manager {
private:
    const int work_time_;
    std::weak_ptr<AccountingSystem> acc_sys_;
    std::weak_ptr<Warehouse> warehouse_;
    std::atomic_bool running_{false};
    std::thread thread_;
    std::weak_ptr<TaskDistributionSystem> task_system_;
    std::shared_ptr<TaskDistributionSystem> tds_;

    void run() {
        while (running_) {
            Logger::log("Continue simulation");   


            std::vector<std::shared_ptr<Pallet>> vector_;
            for (int i = 0; i < RandomGenerator::getRandom(5, 5); i++){
                int type = RandomGenerator::getRandom(1, 3);

                int load = 0;
                switch (type)
                {
                case 1:
                    load = RandomGenerator::getRandom(1, 100);
                    break;
                case 2:
                    load = RandomGenerator::getRandom(1, 30);
                case 3:
                    load = RandomGenerator::getRandom(1, 4);
                }
                
                vector_.push_back(std::make_shared<Pallet>(type, load));
            } 
            auto car = std::make_shared<Car>(vector_);
        
        
            int slot = warehouse_.lock()->getReceivingDock().lock()->acceptCar(car);
            Logger::log("Car is accepted by slot " + std::to_string(slot));
            tds_->addTask(std::make_shared<UnloadTask>(slot), 2);
        
        
            auto shippingCar = std::make_shared<ShippingCar>("Moscow", RandomGenerator::getRandom(1, 100));
            Logger::log("Shipping car created");
            slot = warehouse_.lock()->getShippingZone().lock()->acceptCar(shippingCar);
            Logger::log("Shipping car is accepted by slot " + std::to_string(slot));
            tds_->addTask(std::make_shared<PackingTask>("Moscow", TypeLoads(0, 2, 3)), 2);
        }
    }

public:
    explicit Manager(int work_time, std::weak_ptr<AccountingSystem> acc_sys, std::weak_ptr<Warehouse> warehouse, std::shared_ptr<TaskDistributionSystem> tds)
        : task_system_(acc_sys.lock()->getTaskDistributionSystem().lock()), 
            acc_sys_(acc_sys), warehouse_(warehouse), work_time_(work_time), tds_(tds) {}

    void start() {
        if (!running_) {
            running_ = true;
            thread_ = std::thread(&Manager::run, this);
        }
    }

    void stop() {
        running_ = false;
        if (thread_.joinable()) thread_.join();
    }

    ~Manager() {
        stop();
    }
};

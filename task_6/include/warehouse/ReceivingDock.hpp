#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include <functional>

#include "accounting_system/AccountingSystem.hpp"
#include "accounting_system/zones/ReceivingDockAccounting.hpp"
#include "entities/Car.hpp"
#include "utils/Logger.hpp"

class Pallet;

class Slot : public std::enable_shared_from_this<Slot> {
    private:
        std::timed_mutex terminal_mutex;
        std::shared_ptr<Car> car;
        std::weak_ptr<AccountingSystem> acc_sys;

    public:
        Slot(std::weak_ptr<AccountingSystem> acc_sys) : 
            acc_sys(acc_sys), 
            car(nullptr) {}

        std::function<void()> getReleaseCallback() {
            std::weak_ptr<Slot> weak_this = shared_from_this();
            return [weak_this]() {
                if (auto slot = weak_this.lock()) {
                    slot->releaseCar();
                }
            };
        }
    
        void releaseCar() {
            Logger::log("Car from slot was released");
            car.reset();
        }
        

        bool useTerminal(std::shared_ptr<Pallet> pallet, int work_time) { 
            std::unique_lock<std::timed_mutex> lock(terminal_mutex, std::chrono::seconds(5));
            if (!lock.owns_lock()) return false;
            
            std::this_thread::sleep_for(std::chrono::seconds(work_time));
            return true;
        }

        std::weak_ptr<Car> getCar() const { return car; }
        void acceptCar(std::shared_ptr<Car> car) {
            Logger::log("New car added to the slot");
            car->registerCallback(getReleaseCallback());
            this->car = std::move(car);
        }
};

class ReceivingDock {
    private:
        std::weak_ptr<AccountingSystem> acc_sys;
        std::vector<std::shared_ptr<Slot>> slots;
        
    public:
        ReceivingDock(std::shared_ptr<AccountingSystem> acc_sys) : acc_sys(acc_sys) {
            auto acc = acc_sys->getReceivingDockAccounting().lock();
            
            int slots_count = acc->getSlotsNumber();
            slots.reserve(slots_count);
            
            for (int i = 0; i < slots_count; ++i) {
                slots.push_back(std::make_shared<Slot>(acc_sys));
            }
        }
        
        std::weak_ptr<Slot> getSlot(int slot) const { 
            if (slot < 0 || slot >= static_cast<int>(slots.size())) 
                return {};
            return slots[slot]; 
        }
        
        int acceptCar(std::shared_ptr<Car> car) {
            for (int i = 0; i < static_cast<int>(slots.size()); i++) {
                if (slots[i]->getCar().lock() == nullptr) {
                    slots[i]->acceptCar(std::move(car));
                    return i;
                }
            }
            return -1;
        }
};

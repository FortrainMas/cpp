#pragma once

#include <optional>
#include <atomic>

#include "simulation/Warehouse/basic_entities/Car.hpp"
#include "simulation/warehouse/AccountingSystem.hpp"
#include "simulation/warehouse/zone_entities/Terminal.hpp"
#include "utils/RandomGenerator.h"
#include "utils/Logger.hpp"

class ReceivingDockTerminal : public Terminal {
    public:
        using Terminal::Terminal;
        std::vector<int> getFreeSlots(const Pallet& pallet, int work_time) {
            std::unique_lock<std::timed_mutex> lock(mutex, std::defer_lock);
            if (lock.try_lock_for(std::chrono::seconds(5))) {
                std::this_thread::sleep_for(std::chrono::seconds(work_time));
                return accounting_system.getFreeStorageSlots(pallet);
            } else {
                return std::vector<int>();
            }
        }   
};

class ReceivingDockTask {
    private:
        int slot_number;
    public:
        ReceivingDockTask(int slot_number) : slot_number(slot_number) {}
        int getSlotNumber() { return slot_number; }
};


class CarSlot {
    private:
        std::atomic_bool free_;
        ReceivingDockTerminal terminal;
        std::optional<std::reference_wrapper<Car>> car;
    public:
        CarSlot(AccountingSystem& acc_sys) : terminal(acc_sys), free_(true) {};
        ReceivingDockTerminal& getTerminal() { return terminal; }

        void acceptCar(Car& new_car) { 
            car = new_car;
            free_ = false;
            
        }

        void releaseCar() {
            car.reset();
            free_ = true;
        }

        bool isFree() { return free_; }
        Car& getCar() { return car->get(); }
};


class ReceivingDock {
    private:
        AccountingSystem& accounting_system;
        std::vector<std::shared_ptr<CarSlot>> car_slots;
    public:
        ReceivingDock(AccountingSystem& acc_sys) : accounting_system(acc_sys) {
            int n = RandomGenerator::getRandom(4, 40);
            for(int i = 0; i < n; i++) {
                car_slots.push_back(std::make_shared<CarSlot>(acc_sys));
            }
        }

        int acceptCar(Car& car) {
            for(int i = 0; i < car_slots.size(); i++) {
                if(car_slots[i]->isFree()) {
                    car_slots[i]->acceptCar(car);
                    Logger::log("Car accepted on slot " + std::to_string(i));
                    accounting_system.createTask(ReceivingDockTask(i));
                    return i;
                }
            }
            return -1;
        }

        std::shared_ptr<CarSlot> getSlot(int slot_number) { return car_slots[slot_number]; }
};

#include <thread>
#include <chrono>
#include <vector>

#include "simulation/warehouse/AccountingSystem.hpp"
#include "simulation/warehouse/Warehouse.hpp"
#include "simulation/Loader.hpp"
#include "utils/Logger.hpp"
#include "utils/RandomGenerator.h"

template<class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

Loader::Loader(AccountingSystem& acc_sys, Warehouse& warehouse) : accounting_system(acc_sys),
                                            warehouse(warehouse),
                                            work_time_(RandomGenerator::getRandom(1, 5)), 
                                            running_(true) {
    Logger::log("Loader is created with work time " + std::to_string(work_time_) + " seconds.");
}

void Loader::start() {
    thread_ = std::thread(&Loader::run, this);
}

void Loader::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
    Logger::log("Loader is stopped.");
}

void Loader::run() {
    while (running_) {
        Logger::log("Loader is working for " + std::to_string(work_time_) + " seconds.");
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(work_time_ * 1s);
        std::optional<Task> task = accounting_system.getTask();
        if (task.has_value()) {
            Logger::log("Loader has received a task.");
            std::visit(Overloaded{
                [&](ReceivingDockTask& task) { do_task(task); }
            }, task.value());
        }
    }
}

void Loader::do_task(ReceivingDockTask& task) {
    Logger::log("Loader has received a ReceivingDockTask.");

    ReceivingDock& receiving_dock = warehouse.getReceivingDock();
    CarSlot& car_slot = *receiving_dock.getSlot(task.getSlotNumber());

    Car& car = car_slot.getCar();
    Pallet pallet = car.takePallet(work_time_);

    std::vector<int> free_slots = car_slot.getTerminal().getFreeSlots(pallet, work_time_);

    StorageZone& storage_zone = warehouse.getStorageZone();
    std::shared_ptr<StorageZoneTerminal> terminal = storage_zone.getTerminal();
    terminal->loadPallet(pallet, free_slots, work_time_);

    Logger::log("Loader has finished a ReceivingDockTask.");
}

Loader::~Loader() { stop(); }

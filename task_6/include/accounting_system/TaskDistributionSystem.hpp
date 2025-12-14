#pragma once

#include <mutex>
#include <memory>
#include <thread>
#include <chrono>
#include <unordered_map>

#include "accounting_system/AccountingSystem.hpp"
#include "warehouse/ReceivingDock.hpp"
#include "warehouse/StorageZone.hpp"

using Task = UnloadTask;

class UnloadTask {
    private:
        int slot_number;
        std::weak_ptr<ReceivingDock> receiving_dock;
        std::weak_ptr<StorageZone> storage_zone;

    public:
        UnloadTask(int slot, std::shared_ptr<ReceivingDock> receiving_dock, std::shared_ptr<StorageZone> storage_zone) :
            slot_number(slot),
            receiving_dock(receiving_dock),
            storage_zone(storage_zone) {}

        void doTask(int work_time) {
            std::shared_ptr<Slot> slot = receiving_dock.lock()->getSlot(slot_number).lock();


            // Check up for car. Use lock. May be not present. Break case
            // Get pallet. May be not present. Break case
            // Try to obtain relocation
            // In case if fails, put pallet back
            // Otherwise, confirm relocation
            // Put pallets on storage
            // Confirm relocation on storage
            // Repeat

            while(true) {
                std::shared_ptr<Car> car = slot->getCar().lock();
                if (car == nullptr) break;


                auto pallet_handle = car->getPallet(work_time);
                if (pallet_handle.pallet == nullptr) break;


                std::unique_ptr<StorageZoneRelocation> relocation = slot->useTerminal(pallet_handle.pallet);
                if (relocation == nullptr) {
                    car->rejectGettingPallet(pallet_handle.index, work_time);
                    break;
                } else {
                    car->confirmGettingPallet(pallet_handle.index);
                }

                storage_zone.lock()->relocate(std::move(relocation), work_time);
                auto terminal = storage_zone.lock()->getTerminal().lock();
                terminal->use(work_time);
            }
        }
};


class PackingTask {
    private:
        std::weak_ptr<AccountingSystem> accounting_system;
        std::string destination;
        std::unordered_map<int, int> shipping_plan;
        
    public:
        void doTask(int work_time) {
            // Finds a free table with accounting system and books it.
            // Take pallet from storage according to the task. If fails unbooks table 
            // Packs the ShippingPallet and marks it with destination town

            // If packed enough, accomplish the task
            // In case he lacks an ability to do the task
            // CASE 1: no more space for Shipping pallet:
            //          he puts storage pallets back. mark task as incompleted and put it back to queue
            // CASE 2: needs more pallet from storage.
            //          he repeats the cycle from the second step

            auto acc_sys = accounting_system.lock();
            if (acc_sys == nullptr) return;

            auto packing_zone = acc_sys->getPackingZone().lock();
            if (packing_zone == nullptr) return;

            int reserved_table = packing_zone->reserveTable();
            if (reserved_table == -1) return;

            auto warehouse = acc_sys->getWarehouse().lock();
            if (warehouse == nullptr) return;

            auto storage_zone = warehouse->getStorageZone().lock();
            if (storage_zone == nullptr) return;

            auto terminal = storage_zone->getTerminal().lock();
            if (terminal == nullptr) return;




        }
};

class ShippingTask {
    private:
        int table;
        int pallet_type;
        int 
    public:
        void doTask(int work) {
            // Task should be set by accounting system as often as actually needed 
            // Checks for the current destinations available
            // Checks for the current ShippingPallets available
            // If there is no common destinataions, 
        }
}



class ShippingTask {
    private:
        std::string destination;
        std::unordered_map<int, int> shipping_plan; // key - load type, value - needed amount to ship
        std::weak_ptr<AccountingSystem> accounting_system;

    public:
        ShippingTask(std::string destination, 
            std::unordered_map<int, int> shipping_plan, 
            std::shared_ptr<AccountingSystem> accounting_system) : 
                destination(destination),
                shipping_plan(shipping_plan),
                accounting_system(accounting_system) {}
        
        
        void do () {
            // Finds a free table
            // Sit there and check if there are pallets ready for shipping
            // If there are some, tries to ship them 
        }
};

class TaskDistributionSystem {
    private:
        std::mutex mutex_;
        std::vector<Task> tasks;

    public:

};

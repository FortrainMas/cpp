#pragma once

#include "accounting_system/tasks/Task.hpp"
#include "accounting_system/AccountingSystem.hpp"
#include "warehouse/Warehouse.hpp"
#include "entities/Car.hpp"

#include "utils/Logger.hpp"

class UnloadTask : public Task {
    private:
        int slot;
    public:
        UnloadTask(int slot) : slot(slot) {};
        void doTask (int work_time, std::weak_ptr<AccountingSystem> acc_sys, std::weak_ptr<Warehouse> warehouse) override {

            Logger::log("Task is started");
            // Try to get car;
            std::shared_ptr<ReceivingDock> receivingDock = warehouse.lock()->getReceivingDock().lock();
            std::shared_ptr<Slot> slotPtr = receivingDock->getSlot(slot).lock();
            if (slotPtr == nullptr) return;
            std::shared_ptr<Car> car = slotPtr->getCar().lock();
            if (car == nullptr) return;

            Logger::log("Car is acquired");
            // Take load
            Car::PalletHandle pallet; ;
            while((pallet = car->getPallet(work_time)).index != -1){
                if (pallet.index == -1) return;
                Logger::log("Pallet is taken");

                // Try to put it into storage
                if (slotPtr->useTerminal(pallet.pallet, work_time) == false) {
                    car->rejectGettingPallet(pallet.index, work_time);
                    acc_sys.lock()->getTaskDistributionSystem().lock()->addTask(std::make_shared<UnloadTask>(slot));
                    return;
                }
                std::shared_ptr<StorageZoneAccounting> storage_zone_accounting = acc_sys.lock()->getStorageZoneAccounting().lock();
                storage_zone_accounting->addPallet(pallet.pallet);
                storage_zone_accounting->logState();
                std::shared_ptr<StorageZone> storage_zone = warehouse.lock()->getStorageZone().lock();
                storage_zone->useTerminal(work_time);
                car->confirmGettingPallet(pallet.index);
                Logger::log("Yo task finished");
            }
        }
};

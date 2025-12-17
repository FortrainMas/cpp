#include "accounting_system/tasks/Task.hpp"
#include "accounting_system/AccountingSystem.hpp"
#include "warehouse/Warehouse.hpp"
#include "entities/Car.hpp"

class UnloadTask : public Task {
    private:
        int slot;
    public:
        UnloadTask(int slot) : slot(slot) {};
        void doTask (int work_time, std::weak_ptr<AccountingSystem> acc_sys, std::weak_ptr<Warehouse> warehouse) override {
            // Try to get car;
            std::shared_ptr<ReceivingDock> receivingDock = warehouse.lock()->getReceivingDock().lock();
            std::shared_ptr<Slot> slotPtr = receivingDock->getSlot(slot).lock();
            if (slotPtr == nullptr) return;
            std::shared_ptr<Car> car = slotPtr->getCar().lock();
            if (car == nullptr) return;

            // Take load
            Car::PalletHandle pallet = car->getPallet(work_time);

            // Try to put it into storage
            if (slotPtr->useTerminal(pallet.pallet, work_time) == false) {
                car->rejectGettingPallet(pallet.index, work_time);
                return;
            }
            std::shared_ptr<StorageZoneAccounting> storage_zone_accounting = acc_sys.lock()->getStorageZoneAccounting().lock();
            storage_zone_accounting->addPallet(pallet.pallet);
            std::shared_ptr<StorageZone> storage_zone = warehouse.lock()->getStorageZone().lock();
            storage_zone->useTerminal(work_time);
            car->confirmGettingPallet(pallet.index);
        }
};

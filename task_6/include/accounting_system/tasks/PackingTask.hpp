#include "accounting_system/tasks/Task.hpp"
#include "accounting_system/AccountingSystem.hpp"
#include "warehouse/Warehouse.hpp"
#include "entities/Car.hpp"
#include "entities/TypeLoads.hpp"

class PackingTask : public Task {
    private:
        std::string destination;
        TypeLoads needs;

    public:
        PackingTask(std::string destination, TypeLoads needs) : 
            destination(destination), needs(needs)  {};
        void doTask (int work_time, std::weak_ptr<AccountingSystem> acc_sys, std::weak_ptr<Warehouse> warehouse) override {
            // Try to reserve the table
            std::shared_ptr<PackingZoneAccounting> packing_zone_accounting = acc_sys.lock()->getPackingZoneAccounting().lock();
            int table_number;
            if ((table_number = packing_zone_accounting->reserveTable(work_time)) == -1) return;
            std::shared_ptr<Table> table = warehouse.lock()->getPackingZone().lock()->getTable(table_number).lock();

            // Check free space and use it
            int free_slots = table->getFreeSlots();
            std::shared_ptr<StorageZone> storage_zone = warehouse.lock()->getStorageZone().lock();
            std::shared_ptr<StorageZoneAccounting> storage_zone_accounting = acc_sys.lock()->getStorageZoneAccounting().lock();
            for(int i = 0; i < free_slots; i++) {
                std::shared_ptr<Pallet> pallet = storage_zone_accounting->getPallet(needs);
                if (pallet == nullptr) break;
                storage_zone->useTerminal(work_time);
                table->put_pallet(pallet);
                table->useTerminal(work_time);
            }

            // AssemblePallets

            
            
        }
};

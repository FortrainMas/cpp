#pragma once

#include "accounting_system/tasks/Task.hpp"
#include "accounting_system/AccountingSystem.hpp"
#include "warehouse/Warehouse.hpp"
#include "entities/Car.hpp"
#include "entities/TypeLoads.hpp"

#include "accounting_system/zones/PackingZoneAccounting.hpp"
#include "accounting_system/zones/StorageZoneAccounting.hpp"
#include "accounting_system/zones/ShippingZoneAccounting.hpp"

#include "utils/Logger.hpp"

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
            if ((table_number = packing_zone_accounting->reserveTable(work_time)) == -1) {
                acc_sys.lock()->getTaskDistributionSystem().lock()->addTask(std::make_shared<PackingTask>(destination, needs));
                Logger::log("Failed to reserve the table!");
                return;
            }
            Logger::log("Table was reserved. Cool");
            std::shared_ptr<Table> table = warehouse.lock()->getPackingZone().lock()->getTable(table_number).lock();
            

            do {
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
                    Logger::log("Moved pallet to the table " + std::to_string(table_number));
                }

                // AssemblePallets
                needs = table->assemblePallet(work_time, needs, destination);
                Logger::log("Now we need less loads: " + std::to_string(needs.type1_load) 
                + ", " + std::to_string(needs.type2_load) + ", " + std::to_string(needs.type3_load));

                // Ship palets
                table->useTerminal(work_time);
                std::shared_ptr<ShippingZoneAccounting> shipping_zone_accounting = acc_sys.lock()->getShippingZoneAccounting().lock();
                std::shared_ptr<ShippingZone> shipping_zone = warehouse.lock()->getShippingZone().lock();
                std::shared_ptr<std::set<std::string>> available_destinations = shipping_zone_accounting->getDestinations().lock();

                std::vector<std::shared_ptr<Pallet>> assembled_pallets = table->getAssembledPallets();
                std::vector<int> removed_assembled_pallets;
                for (int i = 0; i < assembled_pallets.size(); i++) {
                    std::shared_ptr<Pallet> asm_pallet = assembled_pallets[i];
                    if (asm_pallet == nullptr) continue;
                    std::shared_ptr<ShippingSlot> shipping_slot = shipping_zone->getSlot(destination).lock();
                    if (shipping_slot == nullptr) continue;
                    std::shared_ptr<ShippingCar> shipping_car = shipping_slot->getCar().lock();
                    if (shipping_car == nullptr) continue;

                    bool result = shipping_car->putPallet(work_time);
                    if (!result) continue;
                    removed_assembled_pallets.push_back(i);

                }
                table->removeAssembledPallet(removed_assembled_pallets);
                removed_assembled_pallets.clear();


                // Remove useless palets
                std::vector<std::shared_ptr<Pallet>> disassemble_pallets = table->getDisassemblePallets();
                for (int i = 0; i < disassemble_pallets.size(); i++) {
                    std::shared_ptr<Pallet> dis_pallet = disassemble_pallets[i];
                    if (dis_pallet == nullptr) continue;
                    std::map<int, int> needs_map = {{1, needs.type1_load}, {2, needs.type2_load}, {3, needs.type3_load}};
                    if((dis_pallet->getLoad() == 0) || (needs_map[dis_pallet->getType()] == 0)) {
                        removed_assembled_pallets.push_back(i);
                        storage_zone->useTerminal(work_time);
                        storage_zone_accounting->addPallet(dis_pallet);
                        continue;
                    }
                }
                table->removeDisassemblePallet(removed_assembled_pallets);
                removed_assembled_pallets.clear();
            } while (table->getFreePackingSlots() > 0 && (needs.type1_load > 0 || needs.type2_load > 0 || needs.type3_load > 0));
        }
};

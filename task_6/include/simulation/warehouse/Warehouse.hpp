#pragma once

#include "simulation/warehouse/AccountingSystem.hpp"

#include "simulation/warehouse/zone_entities/zones/StorageZone.hpp"
#include "simulation/warehouse/zone_entities/zones/ReceivingDock.hpp"

class Warehouse {
    private:
        AccountingSystem accounting_system;
        ReceivingDock receiving_dock;
        StorageZone storage_zone;
    public:
        Warehouse() : accounting_system(), receiving_dock(accounting_system), storage_zone(accounting_system) {};
        AccountingSystem& getAccountingSystem() { return accounting_system; }
        ReceivingDock& getReceivingDock() { return receiving_dock; }
        StorageZone& getStorageZone() { return storage_zone; }
};

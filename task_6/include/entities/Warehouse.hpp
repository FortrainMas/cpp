#pragma once

#include "entities/AccountingSystem.hpp"
#include "entities/StorageZone.hpp"

class Warehouse {
    private:
        AccountingSystem accounting_system;
        StorageZone storage_zone;
    public:
        Warehouse();
        
}
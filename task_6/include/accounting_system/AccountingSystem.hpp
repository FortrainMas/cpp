#pragma once

#include <optional>
#include <vector>
#include <memory>

#include "entities/Pallet.hpp"
#include "entities/Table.hpp"
#include "entities/Car.hpp"
#include "accounting_system/AccountingSystemConfig.hpp"
#include "accounting_system/StorageZoneRelocation.hpp"
#include "accounting_system/PackingZoneAccounting.hpp"

class RecevingDockAccounting {
    private:
        int slots_number;
        std::vector<std::optional<Car>> cars;

    public:
        RecevingDockAccounting(int slots_number) : slots_number(slots_number), cars(slots_number) {}

        int getSlotsNumber() const { return slots_number; }
};

class StorageZoneAccounting {
    private:
        int pallet_slots_number;
        std::vector<std::optional<Pallet>> pallets;

    public:
        StorageZoneAccounting(int pallet_slots_number) : pallet_slots_number(pallet_slots_number), pallets(pallet_slots_number) {};

        std::unique_ptr<StorageZoneRelocation> getRelocation(Pallet pallet);    
};



class ShippingZoneAccounting {
    private:
        int slots_number;
        std::vector<std::optional<Car>> cars;
    
    public:
        ShippingZoneAccounting(int slots_number) : slots_number(slots_number), cars(slots_number) {}
};


class AccountingSystem {
    private:
        std::shared_ptr<RecevingDockAccounting> receiving_dock_accounting;
        std::shared_ptr<StorageZoneAccounting> storage_zone_accounting;
        std::shared_ptr<PackingZoneAccounting> packing_zone_accounting;
        std::shared_ptr<ShippingZoneAccounting> shipping_zone_accounting;

    public:
        AccountingSystem(const AccountingSystemConfig& config) : 
            receiving_dock_accounting(std::make_shared<RecevingDockAccounting>(config.getRecevingDockSlotsNumber())), 
            storage_zone_accounting(std::make_shared<StorageZoneAccounting>(config.getStorageZonePalletSlotsNumber())),
            packing_zone_accounting(std::make_shared<PackingZoneAccounting>(config.getPackingZoneNumTables())),
            shipping_zone_accounting(std::make_shared<ShippingZoneAccounting>(config.getShippingZoneSlotsNumber())) {}
        
        std::weak_ptr<RecevingDockAccounting> getReceivingDockAccounting() const { return receiving_dock_accounting; }
        std::weak_ptr<StorageZoneAccounting> getStorageZoneAccounting() const { return storage_zone_accounting; }
        std::weak_ptr<PackingZoneAccounting> getPackingZoneAccounting() const { return packing_zone_accounting; }
        std::weak_ptr<ShippingZoneAccounting> getShippingZoneAccounting() const { return shipping_zone_accounting; }
};

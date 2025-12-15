#pragma once

#include <optional>
#include <vector>
#include <memory>


#include "accounting_system/AccountingSystemConfig.hpp"
#include "accounting_system/zones/ReceivingDockAccounting.hpp"
#include "accounting_system/zones/StorageZoneAccounting.hpp"
#include "accounting_system/zones/PackingZoneAccounting.hpp"
#include "accounting_system/zones/ShippingZoneAccounting.hpp"




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

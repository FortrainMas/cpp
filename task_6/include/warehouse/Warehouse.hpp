#pragma once

#include <memory>

#include "warehouse/PackingZone.hpp"
#include "warehouse/ReceivingDock.hpp"
#include "warehouse/ShippingZone.hpp"
#include "warehouse/StorageZone.hpp"

#include "accounting_system/AccountingSystem.hpp"
#include "accounting_system/zones/StorageZoneAccounting.hpp"
#include "accounting_system/zones/PackingZoneAccounting.hpp"
#include "warehouse/WarehouseConfig.hpp"
#include "warehouse/StorageZone.hpp"

class Warehouse {
    private:
        std::shared_ptr<AccountingSystem> accounting_system;

        std::shared_ptr<ReceivingDock> receiving_dock;
        std::shared_ptr<StorageZone> storage_zone;
        std::shared_ptr<PackingZone> packing_zone;
        std::shared_ptr<ShippingZone> shipping_zone;
    public:
        Warehouse(WarehouseConfig warehouse_config)
            : accounting_system(std::make_shared<AccountingSystem>(
                  warehouse_config.getAccountingSystemConfig()
              )),
              receiving_dock(std::make_shared<ReceivingDock>(accounting_system)),
              storage_zone(std::make_shared<StorageZone>(accounting_system->getStorageZoneAccounting().lock()->getNumTerminals())),
              packing_zone(std::make_shared<PackingZone>(accounting_system->getPackingZoneAccounting().lock()->getNumTables())),
              shipping_zone(std::make_shared<ShippingZone>(accounting_system))
        {}


        std::weak_ptr<ReceivingDock> getReceivingDock() const { return receiving_dock; }
        std::weak_ptr<StorageZone> getStorageZone() const { return storage_zone; }
        std::weak_ptr<PackingZone> getPackingZone() const { return packing_zone; }
        std::weak_ptr<ShippingZone> getShippingZone() const { return shipping_zone; }
        std::weak_ptr<AccountingSystem> getAccountingSystem() const { return accounting_system; }
};

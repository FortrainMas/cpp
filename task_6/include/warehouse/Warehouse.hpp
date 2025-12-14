#include <memory>

#include "warehouse/PackingZone.hpp"
#include "warehouse/ReceivingDock.hpp"
#include "warehouse/ShippingZone.hpp"
#include "warehouse/StorageZone.hpp"

#include "accounting_system/AccountingSystem.hpp"
#include "warehouse/WarehouseConfig.hpp"


class Warehouse {
    private:
        std::shared_ptr<AccountingSystem> accounting_system;

        PackingZone packing_zone;
        ReceivingDock receiving_dock;
        ShippingZone shipping_zone;
        StorageZone storage_zone;
    public:
        Warehouse(WarehouseConfig warehouse_config)
            : accounting_system(std::make_shared<AccountingSystem>(
                  warehouse_config.getAccountingSystemConfig()
              )),
              packing_zone(accounting_system),
              receiving_dock(accounting_system),
              shipping_zone(accounting_system),
              storage_zone(accounting_system)
        {}
};

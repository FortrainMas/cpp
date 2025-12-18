#include <iostream>

#include "accounting_system/AccountingSystem.hpp"
#include "accounting_system/AccountingSystemConfig.hpp"
#include "warehouse/Warehouse.hpp"
#include "workers/Loader.hpp"

int main(){
    std::cout << "Here is the long story begins..." << std::endl;

    AccountingSystemConfig config(10, 10, 10, 10);
    WarehouseConfig warehouse_config(config);
    std::shared_ptr<Warehouse> warehouse = std::make_shared<Warehouse>(warehouse_config);
    std::shared_ptr<AccountingSystem> accounting_system = std::make_shared<AccountingSystem>(config);

    std::shared_ptr<Loader> loader = std::make_shared<Loader>(1, accounting_system, warehouse);
    loader->start();


    std::shared_ptr<TaskDistributionSystem> tds = accounting_system->getTaskDistributionSystem().lock();
    std::cout << "And here might it continue..." << std::endl;

    return 0;
}

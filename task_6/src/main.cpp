#include <iostream>
#include <thread>
#include <chrono>

#include "accounting_system/AccountingSystem.hpp"
#include "accounting_system/AccountingSystemConfig.hpp"
#include "warehouse/Warehouse.hpp"
#include "workers/Loader.hpp"

#include "accounting_system/tasks/UnloadTask.hpp"
#include "accounting_system/tasks/PackingTask.hpp"

#include "utils/Logger.hpp"


int main(){
    Logger::init();

    Logger::log("Start simulation");

    AccountingSystemConfig config(10, 10, 10, 10);
    WarehouseConfig warehouse_config(config);
    std::shared_ptr<Warehouse> warehouse = std::make_shared<Warehouse>(warehouse_config);
    std::shared_ptr<AccountingSystem> accounting_system = std::make_shared<AccountingSystem>(config);

    std::shared_ptr<Loader> loader = std::make_shared<Loader>(1, accounting_system, warehouse);
    std::shared_ptr<Loader> loader_2 = std::make_shared<Loader>(1, accounting_system, warehouse);
    loader->start();
    loader_2->start();


    std::shared_ptr<TaskDistributionSystem> tds = accounting_system->getTaskDistributionSystem().lock();
    Logger::log("Continue simulation");   

    auto car = std::make_shared<Car>(
        std::vector<std::shared_ptr<Pallet>>{
            std::make_shared<Pallet>(3, 4),
            std::make_shared<Pallet>(3, 4),
            std::make_shared<Pallet>(3, 4),
        }
    );


    int slot = warehouse->getReceivingDock().lock()->acceptCar(car);
    Logger::log("Car is accepted by slot " + std::to_string(slot));
    tds->addTask(std::make_shared<UnloadTask>(slot), 2);


    auto shippingCar = std::make_shared<ShippingCar>("Moscow");
    Logger::log("Shipping car created");
    slot = warehouse->getShippingZone().lock()->acceptCar(shippingCar);
    Logger::log("Shipping car is accepted by slot " + std::to_string(slot));
    // tds->addTask(std::make_shared<PackingTask>("Moscow", TypeLoads(0, 2, 3)), 2);

    char c;
    while (std::cin >> c) {
        if (c == 'q') {
            Logger::log("Do vstrechi na tanzpole");
            Logger::shutdown();
            std::exit(0);
        }
    }
    return 0;
}

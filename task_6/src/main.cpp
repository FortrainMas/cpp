#include <chrono>
#include <thread>

#include "simulation/warehouse/Warehouse.hpp"
#include "simulation/warehouse/Task.hpp"
#include "simulation/warehouse/AccountingSystem.hpp"
#include "simulation/Loader.hpp"
#include "simulation/warehouse/Warehouse.hpp"
#include "utils/RandomGenerator.h"
#include "utils/Logger.hpp"

using namespace std::chrono_literals;

int main() {

    Warehouse warehouse;

    Loader loader(warehouse.getAccountingSystem(), warehouse);
    loader.start();

    Car car({Pallet(1, 4), Pallet(1, 5), Pallet(1, 3), Pallet(1, 4), Pallet(1, 5)});
    warehouse.getReceivingDock().acceptCar(car);

    warehouse.getAccountingSystem().createTask(Task());

    std::this_thread::sleep_for(1200s);

    loader.stop();

    return 0;
}

#include <chrono>

#include "simulation/warehouse/Task.hpp"
#include "simulation/warehouse/AccountingSystem.hpp"
#include "simulation/Loader.hpp"
#include "simulation/warehouse/Warehouse.hpp"
#include "utils/RandomGenerator.h"
#include "utils/Logger.hpp"

using namespace std::chrono_literals;

int main() {

    Warehouse warehouse;

    Loader loader(warehouse.getAccountingSystem());
    loader.start();

    warehouse.getAccountingSystem().createTask(Task());

    std::this_thread::sleep_for(20s);

    loader.stop();

    return 0;
}

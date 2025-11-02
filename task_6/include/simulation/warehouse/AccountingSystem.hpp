#pragma once

#include <mutex>
#include <vector>
#include <optional>

#include "simulation/warehouse/Task.hpp"
#include "simulation/warehouse/basic_entities/Pallet.hpp"
#include "simulation/warehouse/zone_entities/zones/StorageZone.hpp"

class AccountingSystem {
    private:
        std::mutex pallets_mutex;
        std::vector<Task> tasks;
        StorageZonePlaces storage_zone_places;
    public:
        AccountingSystem() : tasks(), pallets_mutex(), storage_zone_places() {};

        StorageZonePlaces& getStorageZonePlaces() { return storage_zone_places; }

        std::optional<Task> getTask();
        void createTask(const Task& task);

        std::vector<int> getFreeStorageSlots(const Pallet& pallet);
};

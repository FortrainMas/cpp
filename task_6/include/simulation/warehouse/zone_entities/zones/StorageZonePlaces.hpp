#pragma once

#include <vector>
#include <optional>

#include "simulation/warehouse/basic_entities/Pallet.hpp"
#include "utils/RandomGenerator.h"
#include "utils/Logger.hpp"

class StorageZonePlaces {
    private:
        std::vector<std::optional<Pallet>> places;
    public:
        StorageZonePlaces() : places() {
            int racks = RandomGenerator::getRandom(10, 500);
            Logger::log("Creating " + std::to_string(racks) + " racks in storage zone.");
            for(int i = 0; i < racks * 10 * 6; i++) {
                places.push_back(std::nullopt);
            }
            Logger::log("Storage zone created. With " + std::to_string(places.size()) + " places.");
        }

        std::vector<std::optional<Pallet>>& getPlaces() { return places; }
};

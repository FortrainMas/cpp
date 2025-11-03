#include <vector>
#include <numeric>
#include <mutex>
#include <optional>

#include "simulation/warehouse/Task.hpp"
#include "simulation/warehouse/basic_entities/Pallet.hpp"
#include "simulation/warehouse/AccountingSystem.hpp"

std::optional<Task> AccountingSystem::getTask() {
    std::lock_guard<std::mutex> lock(pallets_mutex);

    if (tasks.size() == 0) {
        return {};
    }

    Task& first = tasks.front();
    tasks.erase(tasks.begin());
    return first;
}

void AccountingSystem::createTask(const Task& task) {
    std::lock_guard<std::mutex> lock(pallets_mutex);
    tasks.push_back(task);
}


std::vector<int> AccountingSystem::getFreeStorageSlots(const Pallet& pallet) {
    std::lock_guard<std::mutex> lock(pallets_mutex);
    std::vector<std::optional<Pallet>>& places = storage_zone_places.getPlaces();
    std::vector result = std::vector<int>();


    bool is_enough_space = false;
    int max_added_weight = 0;
    for(int i = 0; i < places.size(); i++) {
        if (places[i].has_value()) {
            Pallet& warehouse_pallet = places[i].value();
            if(warehouse_pallet.getType() == pallet.getType()) {
                max_added_weight += warehouse_pallet.getMaxLoad() - warehouse_pallet.getLoad();
            }
            if (max_added_weight < pallet.getLoad()) {
                is_enough_space = true;
                break;
            }
        } else {
            is_enough_space = true;
            break;
        }
    }

    if(!is_enough_space) {
        throw std::runtime_error("Storage zone is full. Can't accomplish task. Warehouse exploded.");
    }


    int pallet_weight = pallet.getLoad();
    for(int i = 0; i < places.size(); i++) {
        if (places[i].has_value()) {
            Pallet& warehouse_pallet = places[i].value();
            if(warehouse_pallet.getType() == pallet.getType()) {
                int added_weight = std::min(pallet_weight, warehouse_pallet.getMaxLoad() - warehouse_pallet.getLoad());
                warehouse_pallet.addLoad(added_weight);
                pallet_weight -= added_weight;
                result.push_back(i);

                if(pallet_weight == 0) {
                    return result;
                }
            }     
        }
    }

    for(int i = 0; i < places.size(); i++) {
        if (!places[i].has_value()) {
            places[i] = pallet;
            result.push_back(i);
            return result;
        }
    }

    return result;
}

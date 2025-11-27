#include <vector>
#include <numeric>
#include <mutex>
#include <optional>

#include "./entities.hpp"


AccountingSystem::AccountingSystem (const std::vector<int> racks_capacities) {
    int pallets_size = 10 * std::accumulate(racks_capacities.begin(), racks_capacities.end(), 1, std::multiplies<int>());
    pallets = std::vector<std::optional<Pallet>>(pallets_size);
}

void AccountingSystem::releasePlace(int position) {
    std::lock_guard<std::mutex> lock(pallets_mutex);
}

int AccountingSystem::reservePlace(const Pallet &pallet) {
    std::lock_guard<std::mutex> lock(pallets_mutex);

    for(size_t i = 0; i < pallets.size(); i++) {
        if(pallets[i].has_value()) {
            Pallet& warehouse_pallet = pallets[i].value();
            if(fits(warehouse_pallet, pallet)) {
                warehouse_pallet.addLoad(pallet.getLoad());
                return static_cast<int>(i);
            }
            continue;
        }
        
        pallets[i] = pallet;
        return static_cast<int>(i);
    }

    return -1;
}

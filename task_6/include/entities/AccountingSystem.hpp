#include <vector>
#include <numeric>
#include <mutex>
#include <optional>

#include "entities/Pallet.hpp"


class AccountingSystem {
    private:
        std::vector<std::optional<Pallet>> pallets;
        std::mutex pallets_mutex;
    public:
        AccountingSystem(const std::vector<int> racks_capacities) {
            int pallets_size = 10 * std::accumulate(racks_capacities.begin(), racks_capacities.end(), 1, std::multiplies<int>());
            pallets = std::vector<std::optional<Pallet>>(pallets_size);
        }
};

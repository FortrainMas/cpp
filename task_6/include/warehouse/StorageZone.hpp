#include <memory>
#include <mutex>

#include "accounting_system/AccountingSystem.hpp"

class StorageZoneTerminal {
    private:
        std::mutex mutex;
    public:
        StorageZoneTerminal() {}

        void use const (int work_time) {
            std::lock_guard<std::mutex> lock(mutex);

        }

        void get_unload const (std::unordered_map<int, int> const &shipping_plan) {

        }
};


class StorageZone {
    private:
        int num_terminals;
        std::vector<std::shared_ptr<StorageZoneTerminal>> terminals;

        std::weak_ptr<AccountingSystem> accounting_system;
        std::vector<std::optional<std::shared_ptr<Pallet>>> pallets;
    
    public:
        StorageZone(std::shared_ptr<AccountingSystem> acc_sys) : accounting_system(acc_sys) {}

        void relocate(std::unique_ptr<StorageZoneRelocation> relocation, int work_time);

        std::weak_ptr<StorageZoneTerminal> getTerminal() {}
};

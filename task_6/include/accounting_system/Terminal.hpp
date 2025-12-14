#include <memory>

#include "accounting_system/AccountingSystem.hpp"
#include "zones/StorageZoneAccounting.hpp"

class Terminal {
    private:
        std::weak_ptr<AccountingSystem> accounting_system;

    public:
        Terminal(std::weak_ptr<AccountingSystem> accounting_system) : accounting_system(accounting_system) {}

        std::unique_ptr<StorageZoneRelocation> getRelocation(std::shared_ptr<Pallet> pallet) const;
        std::unique_ptr<StorageZoneRelocation> getRelocation()  


}
#include <memory>

#include "accounting_system/AccountingSystem.hpp"

class PackingZoneTerminal {
    public:
        std::weak_ptr<AccountingSystem> acc_sys;
    
    private:
        void use(std::shared_ptr<Pallet> pallet) {}
}


class PackingZone {
    private:
        std::weak_ptr<AccountingSystem> accounting_system;
        std::shared_ptr<Table> tables;
    
    public:
        PackingZone(std::shared_ptr<AccountingSystem> accounting_system) : accounting_system(accounting_system) {}
};


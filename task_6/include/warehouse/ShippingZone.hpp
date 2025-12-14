#include <memory>

#include "accounting_system/AccountingSystem.hpp"

class ShippingZone {
    private:
        std::weak_ptr<AccountingSystem> accounting_system;
        
    public:
        ShippingZone(std::shared_ptr<AccountingSystem> acc_sys) : accounting_system(acc_sys) {}
};

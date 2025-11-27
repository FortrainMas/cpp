#pragma once

#include <mutex>

#include "simulation/warehouse/AccountingSystem.hpp"

class Terminal {
    protected:
        std::timed_mutex mutex;
        AccountingSystem& accounting_system;
    public:
        explicit Terminal(AccountingSystem& acc_sys) 
            : accounting_system(acc_sys), mutex() {};
};

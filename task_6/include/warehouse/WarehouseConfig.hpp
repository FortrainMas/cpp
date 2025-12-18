#pragma once

#include "accounting_system/AccountingSystemConfig.hpp"

class WarehouseConfig {
    private:
        AccountingSystemConfig accounting_system_config;
    public:
        WarehouseConfig(AccountingSystemConfig accounting_system_config) : accounting_system_config(accounting_system_config) {}

        AccountingSystemConfig getAccountingSystemConfig() const { return accounting_system_config; }
};

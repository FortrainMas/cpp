#pragma once

#include <memory>

class AccountingSystem;
class Warehouse;

class Task {
    public:
        virtual ~Task() = default;
        virtual void doTask(int work_time, std::weak_ptr<AccountingSystem> acc_sys, std::weak_ptr<Warehouse> warehouse) = 0;
};

#pragma once

#include <thread>
#include <atomic>

#include "simulation/warehouse/zone_entities/zones/ReceivingDock.hpp"
#include "simulation/warehouse/AccountingSystem.hpp"

class Loader {
    private:
        const int work_time_;
        std::atomic_bool running_;
        std::thread thread_;
        AccountingSystem& accounting_system;
        Warehouse& warehouse;

        void run();
        void do_task(ReceivingDockTask& task);

    public:
        explicit Loader(AccountingSystem& acc_sys, Warehouse& warehouse);
        void start();
        void stop();
        ~Loader();
};

#pragma once

#include <vector>
#include <optional>
#include <memory>
#include <chrono>

#include "simulation/warehouse/AccountingSystem.hpp"
#include "simulation/warehouse/zone_entities/Terminal.hpp"
#include "simulation/warehouse/basic_entities/Pallet.hpp"
#include "simulation/warehouse/zone_entities/zones/StorageZonePlaces.hpp"
#include "utils/RandomGenerator.h"
#include "utils/Logger.hpp"

class StorageZoneTerminal : public Terminal {
    public:
        using Terminal::Terminal;
        void loadPallet(Pallet& pallet, std::vector<int> free_slots, int work_time) {
            std::unique_lock<std::timed_mutex> lock(mutex, std::defer_lock);
            lock.lock();
            std::this_thread::sleep_for(std::chrono::seconds(work_time));
        }
        bool test_free(){
            if(mutex.try_lock()) {
                mutex.unlock();
                return true;
            }
            return false;
        }
};

class StorageZone {
    private:
        StorageZonePlaces& places;
        AccountingSystem& accounting_system;
        std::vector<std::shared_ptr<StorageZoneTerminal>> terminals;
    public:
        StorageZone(AccountingSystem& acc_sys) 
                    : accounting_system(acc_sys),
                     places(acc_sys.getStorageZonePlaces()),
                     terminals(RandomGenerator::getRandom(1, 10)) {}
        void loadPallet(Pallet& pallet, std::vector<int> free_slots, int work_time) {
            std::this_thread::sleep_for(std::chrono::seconds(work_time));
        }

        std::shared_ptr<StorageZoneTerminal> getTerminal() {
            for(int i = 0; i < terminals.size(); i++) {
                if(terminals[i]->test_free()) {
                    return terminals[i];
                }
            }
            return terminals[0];
        }
        
};

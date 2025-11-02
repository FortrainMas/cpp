#include <vector>
#include <optional>

#include "simulation/warehouse/basic_entities/Pallet.hpp"
#include "simulation/warehouse/AccountingSystem.hpp"
#include "utils/RandomGenerator.h"
#include "utils/Logger.hpp"

class StorageZonePlaces {
    private:
        std::vector<std::optional<Pallet>> places;
    public:
        StorageZonePlaces() : places() {
            int racks = RandomGenerator::getRandom(10, 500);
            Logger::log("Creating " + std::to_string(racks) + " racks in storage zone.");
            for(int i = 0; i < racks * 10 * 6; i++) {
                places.push_back(std::nullopt);
            }
            Logger::log("Storage zone created. With " + std::to_string(places.size()) + " places.");
        }

        std::vector<std::optional<Pallet>>& getPlaces() { return places; }
};

class StorageZoneTerminal : public Terminal {
    public:
        using Terminal::Terminal;
        void loadPallet(Pallet& pallet, std::vector<int> free_slots, int work_time) {
            std::lock_guard<std::mutex> lock(mutex);
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
        }
        
};

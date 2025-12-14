#include <vector>
#include <mutex>
#include <memory>

#include "entities/Pallet.hpp"

class TableAccounting {
    private:
        std::mutex mutex;
        bool reserved;

        std::vector<std::weak_ptr<PackedPallet>> pallets;

    public:
        TableAccounting() : reserved(false) {}

        void reserve() { reserved = true; }
        void release() { reserved = false; }
        bool isReserved() { return reserved; }

        void take_load(int position) {
            
        }
};


class PackingZoneAccounting {
    private:
        std::mutex mutex;
        int num_tables;
        std::vector<TableAccounting> tables;
    
    public:
        PackingZoneAccounting(int num_tables) : num_tables(num_tables), tables(num_tables) {}

        int reserveTable() {
            std::lock_guard<std::mutex> lock(mutex);

            for (int i = 0; i < num_tables; i++) {
                if (!tables[i].isReserved()) {
                    tables[i].reserve();
                    return i;
                }
            }
            return -1;
        }
};

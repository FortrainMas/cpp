#pragma once

#include <vector>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <chrono>

#include "entities/Pallet.hpp"

class PackingZoneAccounting {
private:
    std::mutex mutex;
    std::condition_variable cv;
    int num_tables;
    std::vector<bool> reserved_tables;
    
public:
    PackingZoneAccounting(int num_tables) 
        : num_tables(num_tables), reserved_tables(num_tables, false) {}

    int reserveTable(int work_time) {
        std::unique_lock<std::mutex> lock(mutex);
        
        bool success = cv.wait_for(lock, std::chrono::seconds(5), [this] {
            for (bool reserved : reserved_tables) {
                if (!reserved) return true;
            }
            return false;
        });
        
        if (!success) {
            return -1;
        }
        
        
        for (int i = 0; i < num_tables; i++) {
            if (!reserved_tables[i]) {
                reserved_tables[i] = true;
                return i;
            }
        }
        
        return -1;
    }

    void releaseTable(int table) {
        std::lock_guard<std::mutex> lock(mutex);
        if (table >= 0 && table < num_tables) {
            reserved_tables[table] = false;
            cv.notify_all(); 
        }
    }

    int getNumTables() const {
        return num_tables;
    }
};

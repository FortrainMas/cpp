#include <vector>
#include <atomic>

#include <simulation/warehouse/basic_entities/Pallet.hpp>
#include <utils/Semaphore.hpp>

class Car {
    private:
        Semaphore sem;
        std::vector<Pallet> pallets;
    public:
        Car(std::vector<Pallet> &&pallets) : pallets(pallets), sem(3) {};
        Pallet takePallet(int work_time) {
            sem.acquire();
            std::this_thread::sleep_for(std::chrono::seconds(work_time));
            Pallet pallet = std::move(pallets.back());
            pallets.pop_back();
            sem.release();
            return pallet;
        }
        Pallet putPallet(Pallet &&pallet, int work_time) {
            sem.acquire();
            std::this_thread::sleep_for(std::chrono::seconds(work_time));
            pallets.push_back(std::move(pallet));
            sem.release();
            return std::move(pallet);
        }
};

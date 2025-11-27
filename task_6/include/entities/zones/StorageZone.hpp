#include <vector>
#include <numeric>
#include <mutex>
#include <optional>
#include <semaphore>

class StorageZone {
    private:
        std::vector<std::optional<Pallet>> pallets;
        std::counting_semaphore<10> sem;
        std::mutex pallets_mutex;
    public:
        StorageZone(std::vector<int> racks_capacities, int terminals_number) {
            sem = std::counting_semaphore<10>(terminals_number);
            pallets_mutex = std::mutex();
            pallets_size = 10 * std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>());
            pallets = std::vector<std::optional<Pallet>>(pallets_size);
        }

        void terminal_checkup(int worker_task_time) {
            sem.acquire();
            sleep(worker_task_time);
            sem.release();
        }

        void load_pallet(Pallet pallet) {
            std::lock_guard<std::mutex> lock(pallets_mutex);
            pallets.push_back(pallet);
        }
};

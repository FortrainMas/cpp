#include <vector>
#include <memory>
#include <mutex>
#include <thread>

#ifdef __linux__
#include <semaphore>
#else
#include <utils/semaphore.h>
#endif

#include <entities/Pallet.hpp>

class Car {
    private:
        std::counting_semaphore<3> sem{3};
        std::mutex mutex;
        std::vector<std::shared_ptr<Pallet>> pallets;
        std::vector<bool> in_progress; 
        std::function<void()> departure_callback;

    public:
        Car(std::vector<std::shared_ptr<Pallet>> pallets) 
            : pallets(std::move(pallets)), 
              in_progress(this->pallets.size(), false) {}

        struct PalletHandle {
            int index;
            std::shared_ptr<Pallet> pallet;
        };

        PalletHandle getPallet(int work_time) {
            sem.acquire();

            std::this_thread::sleep_for(std::chrono::seconds(work_time));

            std::lock_guard<std::mutex> lock(mutex);

            for(int i = 0; i < pallets.size(); i++) {
                if (pallets[i] != nullptr && !in_progress[i]) {
                    in_progress[i] = true;
                    sem.release();
                    return {i, pallets[i]};
                }
            }

            sem.release();
            return {-1, nullptr};
        }

        void confirmGettingPallet(int index) {
            std::lock_guard<std::mutex> lock(mutex);
            if (index >= 0 && index < pallets.size()) {
                pallets[index].reset(); 
                in_progress[index] = false; 

                bool all_empty = true;
                for (const auto& pallet : pallets) {
                    if (pallet) {
                        all_empty = false;
                        break;
                    }
                }

                if (all_empty) {
                    departure_callback();
                }
            }
        }

        void rejectGettingPallet(int index, int work_time) {
            sem.acquire();

            std::this_thread::sleep_for(std::chrono::seconds(work_time));

            std::lock_guard<std::mutex> lock(mutex);

            if (index >= 0 && index < pallets.size()) {
                in_progress[index] = false;
            }

            sem.release();
        }

        void registerCallback(std::function<void()> callback) {
            departure_callback = callback;
        }
};

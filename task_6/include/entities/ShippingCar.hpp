#pragma once

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

class ShippingCar {
    private:
        std::counting_semaphore<3> sem{3};
        int available_size = 100;
        std::function<void(const std::string&)> departure_callback;
        std::mutex mutex;
        std::string destination;

    public:
        ShippingCar(std::string destination) : destination(destination) {}

        bool putPallet(int work_time) {
            sem.acquire();
            std::this_thread::sleep_for(std::chrono::seconds(work_time));
            std::lock_guard<std::mutex> lock(mutex);
            if(available_size == 0) {
                sem.release();
                return false;
            }
            available_size--;
            if (available_size == 0) {
                departure_callback(destination);
            }
            sem.release();
            return true;
        }

        void registerCallback(std::function<void(const std::string&)> callback) {
            departure_callback = callback;
        }

        std::string getDestination() const { return destination; }
};

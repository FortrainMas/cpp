#include <vector>
#include <semaphore>

#include "entities/Pallet.hpp"

class Car {
private:
    std::counting_semaphore<10> sem;
    std::vector<Pallet> pallets;
public: 
    Car(vector<Pallet> &&pallets) {
        sem = std::counting_semaphore<10>(10);
        this->pallets = pallets;
    }

    Pallet takePallet() {
        sem.acquire();
        Pallet pallet = this->pallets.back();
        this->pallets.pop_back();
        sem.release();
        return pallet;
    }

    Pallet putPallet(Pallet &&pallet) {
        sem.acquire();
        this->pallets.push_back(pallet);
        sem.release();
        return pallet;
    }
};

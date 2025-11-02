#include <vector>
#include <semaphore>

#include "./entities.hpp"

Car::Car(vector<Pallet> &&pallets) {
    sem = std::counting_semaphore<10>(10);
    this->pallets = pallets;
}

Pallet Car::takePallet() {
    sem.acquire();
    Pallet pallet = this->pallets.back();
    this->pallets.pop_back();
    sem.release();
    return pallet;
}

Pallet Car::putPallet(Pallet &&pallet) {
    sem.acquire();
    this->pallets.push_back(pallet);
    sem.release();
    return pallet;
}


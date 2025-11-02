#include <stdexcept>

#include "./entities.hpp"

Pallet::Pallet(int type, int load) {
    this->type = type;
    this->load = load;
        
    switch (type) {
        case 1:
            this->max_load = 100;
            break;
        case 2:
            this->max_load = 200;
            break;
        case 3:
            this->max_load = 300;
            break;
        default:
            throw std::invalid_argument("Invalid pallet type");
    }
}

int Pallet::getType () const { return type; }
int Pallet::getLoad () const { return load; }
int Pallet::getMaxLoad () const { return max_load; }

void Pallet::addLoad(int load) {
    if (this->load + load <= this->max_load) {
        this->load += load;
    } else {
        throw std::invalid_argument("Pallet is full");
    }
}

void Pallet::removeLoad(int load) {
    if (this->load - load >= 0) {
        this->load -= load;
    } else {
        throw std::invalid_argument("Pallet is empty");
    }
}

bool Pallet::fits(Pallet& pallet1, Pallet& pallet2) {
    if  (pallet1.type != pallet2.type) {
        return false;
    } else if (pallet1.load + pallet2.load > pallet1.max_load) {
        return false;
    } else {
        return true;
    }
}

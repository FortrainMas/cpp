#pragma once

class Pallet {
    private:
        int type;
        int load;
        int max_load;
    public:
        Pallet(int type, int load) {
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
        
        int getType() const { return type; }
        int getLoad() const { return load; }
        int getMaxLoad() const { return max_load; }
        void addLoad(int load) {
            if (this->load + load <= this->max_load) {
                this->load += load;
            } else {
                throw std::invalid_argument("Pallet is full");
            }
        }
        void removeLoad(int load) {
            if (this->load - load >= 0) {
                this->load -= load;
            } else {
                throw std::invalid_argument("Pallet is empty");
            }
        }

        bool fits(Pallet& pallet) {
            if  (type != pallet.type) {
                return false;
            } else if (load + pallet.load > max_load) {
                return false;
            } else {
                return true;
            }
        }
};


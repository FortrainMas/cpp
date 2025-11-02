#include <mutex>
#include <vector>
#include <semaphore>

class Terminal {
    private:
        std::mutex mutex;
        AccountingSystem accounting_system;
    public:
        explicit Terminal(AccountingSystem& acc_sys) 
            : accounting_system(acc_sys) {};
};

class AccountingSystem {
    private:
        std::mutex pallets_mutex;
        std::vector<std::optional<Pallet>> pallets;
    public:
        AccountingSystem(const std::vector<int> racks_capacities);
        int reservePlace(const Pallet& pallet);
        void releasePlace(int position);
};


class Car {
    private:
        std::counting_semaphore<10> sem;
        std::vector<Pallet> pallets;
    public:
        Car(vector<Pallet> &&pallets);
        Pallet takePallet();
        Pallet putPallet(Pallet &&pallet);
};

class Pallet {
    private:
        int type;
        int load;
        int max_load;
    public:
        Pallet(int type, int load);
        int getType() const { return type; }
        int getLoad() const { return load; }
        int getMaxLoad() const { return max_load; }
        void addLoad(int load);
        void removeLoad(int load);

        bool fits(Pallet& pallet1, Pallet& pallet2);
};

class Warehouse {
    private:
        AccountingSystem accounting_system;
        StorageZone storage_zone;
    public:
        Warehouse();
}

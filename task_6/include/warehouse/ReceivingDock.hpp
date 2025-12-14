#include <memory>
#include <mutex>

#include "accounting_system/AccountingSystem.hpp"

class Car;
class Pallet;

class ReceivingDockTerminal {
    private:
        std::mutex mutex;
        std::weak_ptr<AccountingSystem> acc_sys;

    public:
        ReceivingDockTerminal(std::weak_ptr<AccountingSystem> acc_sys) : acc_sys(acc_sys) {}

        std::unique_ptr<StorageZoneRelocation> use(std::shared_ptr<Pallet> pallet);
        
};

class Slot {
    private:
        std::shared_ptr<Car> car;
        std::weak_ptr<AccountingSystem> acc_sys;
        std::unique_ptr<ReceivingDockTerminal> terminal;

    public:
        Slot(std::weak_ptr<AccountingSystem> acc_sys) : 
            acc_sys(acc_sys), 
            car(nullptr), 
            terminal(std::make_unique<ReceivingDockTerminal>(acc_sys)) {}


        std::unique_ptr<StorageZoneRelocation> useTerminal(std::shared_ptr<Pallet> pallet) const { return terminal->use(pallet); }
        std::unique_ptr<Pallet> getPallet();

        std::weak_ptr<Car> getCar() const { return car; }
        void acceptCar(std::unique_ptr<Car> car);
};

class ReceivingDock {
    private:
        std::weak_ptr<AccountingSystem> acc_sys;
        std::vector<std::shared_ptr<Slot>> slots;
    public:
        ReceivingDock(std::shared_ptr<AccountingSystem> acc_sys) : 
            acc_sys(acc_sys), 
            slots(acc_sys->getReceivingDockAccounting().lock()->getSlotsNumber()) {}

        
        std::weak_ptr<Slot> getSlot(int slot) const { return slots[slot]; }
        void acceptCar(std::unique_ptr<Car> car);
};

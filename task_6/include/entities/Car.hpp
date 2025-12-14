#include <vector>
#include <memory>

#include "entities/Pallet.hpp"
#include "warehouse/ReceivingDock.hpp"


class Car {
    private:
        std::weak_ptr<Slot> slot;
        std::shared_ptr<std::vector<std::shared_ptr<Pallet>>> pallets;
    public:
        Car(std::shared_ptr<std::vector<std::shared_ptr<Pallet>>> pallets) : pallets(pallets) {}

        void setSlot(std::weak_ptr<Slot> slot) {
            this->slot = slot;
        }



        struct PalletHandle {
            int index;
            std::shared_ptr<Pallet> pallet;
        };

        struct PalletHandle getPallet(int work_time);
        void confirmGettingPallet(int index);
        void rejectGettingPallet(int index, int work_time);


};

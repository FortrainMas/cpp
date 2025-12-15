#include <vector>
#include <memory>
#include <mutex>

#include "entities/Pallet.hpp"



using PalletSlotsAccounting = std::vector<std::shared_ptr<Pallet>>;
class StorageZoneAccounting {
    private:
        int terminals_number;
        int pallet_slots_number;
        PalletSlotsAccounting pallets;
        std::mutex mutex;

    public:
        StorageZoneAccounting(int terminals_number,int pallet_slots_number) : 
            terminals_number(terminals_number), 
            pallet_slots_number(pallet_slots_number), 
            pallets(pallet_slots_number) {};

        void addPallet(std::shared_ptr<Pallet> pallet) {
            std::lock_guard<std::mutex> lock(mutex);

            for(int i = 0; i < pallet_slots_number; i++) {
                if ((pallets[i] != nullptr) && (pallets[i]->getType() == pallet->getType())){
                    pallets[i]->addLoad(std::min(pallet->getLoad(),pallets[i]->getMaxLoad() - pallets[i]->getLoad()));
                }
                if(pallet->getLoad() == 0) {
                    return;
                }
            }


            for (int i = 0; i < pallet_slots_number; i++) {
                if (pallets[i] == nullptr) {
                    pallets[i] = pallet;
                    return;
                }
            }
        } 

        std::shared_ptr<Pallet> getPallet(int type, int load) {
            std::lock_guard<std::mutex> lock(mutex);

            int max_present_load = 0;
            for(int i = 0; i < pallet_slots_number; i++) {
                if (pallets[i]->getLoad() >= load && pallets[i]->getType() == type) {
                    auto tmp = pallets[i];
                    pallets[i] = nullptr;
                    return tmp;
                }
                if (pallets[i]->getType() == type) {
                    max_present_load = std::max(max_present_load, pallets[i]->getLoad());
                }
            }
            
            for(int i = 0; i < pallet_slots_number; i++) {
                if (pallets[i]->getType() == type && pallets[i]->getLoad() == max_present_load) {
                    auto tmp = pallets[i];
                    pallets[i] = nullptr;
                    return tmp;
                }
            }
            return nullptr;
        }
};

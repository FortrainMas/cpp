#pragma once

#include <vector>
#include <memory>
#include <mutex>

#include "entities/Pallet.hpp"
#include "entities/TypeLoads.hpp"
#include "utils/Logger.hpp"

class StorageZoneAccounting {
    private:
        int terminals_number;
        int pallet_slots_number;
        std::vector<std::shared_ptr<Pallet>> pallets;
        std::mutex mutex;

    public:
        StorageZoneAccounting(int pallet_slots_number, int terminals_number=5) :
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


        std::shared_ptr<Pallet> getPallet(TypeLoads type_loads) {
            std::lock_guard<std::mutex> lock(mutex);
            int type_1 = type_loads.type1_load;
            int type_2 = type_loads.type2_load;
            int type_3 = type_loads.type3_load;

            int bestfit_index = -1;
            int bestfit_ratio = -1;

            for(int i = 0; i < pallet_slots_number; i++){
                if (pallets[i] == nullptr) continue;

                if (pallets[i]->getType() == 1 && type_1 / pallets[i]->getLoad() > bestfit_ratio) {
                    bestfit_ratio = type_1 / pallets[i]->getLoad();
                    bestfit_index = i;
                }

                else if (pallets[i]->getType() == 2 && type_2 / pallets[i]->getLoad() > bestfit_ratio) {
                    bestfit_ratio = type_2 / pallets[i]->getLoad();
                    bestfit_index = i;
                }

                else if (pallets[i]->getType() == 3 && type_3 / pallets[i]->getLoad() > bestfit_ratio) {
                    bestfit_ratio = type_3 / pallets[i]->getLoad();
                    bestfit_index = i;
                }
            }

            if (bestfit_index != -1) {
                auto tmp = pallets[bestfit_index];
                pallets[bestfit_index] = nullptr;
                return tmp;
            }
            return nullptr;
        }


        int getNumTerminals() const {
            return terminals_number;
        }

        void logState() {
            std::lock_guard<std::mutex> lock(mutex);
            std::string log_message = "Current state of the storage zone: \n";
            for(auto pallet : pallets) {
                if (pallet != nullptr) {
                    log_message.append("Pallet type: " + std::to_string(pallet->getType()) + ", load: " + std::to_string(pallet->getLoad()) + "\n");
                }
            }
            Logger::log(log_message);
        }
};

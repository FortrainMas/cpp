#include <vector>
#include <optional>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>

#include "entities/Pallet.hpp"
#include "entities/TypeLoads.hpp"


using PalletSlots = std::vector<std::shared_ptr<Pallet>>;

class Table {
    private:
        int disassemble_slots_number;
        int packing_slots_number;
        PalletSlots disassemble_pallets;
        PalletSlots packing_pallets;
    public:
        Table(int disassemble_slots_number, int packing_slots_number) :
         disassemble_slots_number(disassemble_slots_number), packing_slots_number(packing_slots_number),
         disassemble_pallets(disassemble_slots_number), packing_pallets(packing_slots_number) {};

        std::shared_ptr<Pallet> get_pallet() {
            for (int i = 0; i < packing_pallets.size(); i++) {
                if (packing_pallets[i] != nullptr) {
                    auto tmp = packing_pallets[i];
                    packing_pallets[i] = nullptr;
                    return tmp;
                }
            }
            return nullptr;
        }

        bool put_pallet(std::shared_ptr<Pallet> pallet) {
            for(int i = 0; i < disassemble_pallets.size(); i++) {
                if (disassemble_pallets[i] == nullptr) {
                    disassemble_pallets[i] = pallet;
                    return true;
                }
            }
            return false;
        }

        
        int getFreeSlots() {
            int free_slots = 0;
            for (int i = 0; i < disassemble_pallets.size(); i++) {
                if (disassemble_pallets[i] == nullptr) {
                    free_slots++;
                }
            }
            return free_slots;
        }

        void assemblePallets() {
            if (getFreeSlots() == disassemble_slots_number) return;
            for (int i = 0; i < packing_pallets.size(); i++) {
                for(int i = 0) {}
            }
        }

        TypeLoads updateNeeds(TypeLoads needs) {
            for(int i = 0; i < disassemble_pallets.size(); i++) {
                if (disassemble_pallets[i] != nullptr) {
                    if (disassemble_pallets[i]->getType() == 1) {needs.type1_load -= disassemble_pallets[i]->getLoad();}
                    else if (disassemble_pallets[i]->getType() == 2) {needs.type2_load -= disassemble_pallets[i]->getLoad();}
                    else {needs.type3_load -= disassemble_pallets[i]->getLoad();}
                }
            }
            return needs;
        }

        void useTerminal(int work_time) {
            std::this_thread::sleep_for(std::chrono::seconds(work_time));
        }
};

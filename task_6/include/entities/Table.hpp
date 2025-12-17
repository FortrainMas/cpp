#include <vector>
#include <optional>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <map>

#include "entities/Pallet.hpp"
#include "entities/TypeLoads.hpp"


class Table {
    private:
        int disassemble_slots_number;
        int packing_slots_number;
        std::vector<std::shared_ptr<Pallet>> disassemble_pallets;
        std::vector<std::shared_ptr<Pallet>> packing_pallets;
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

        void putAssembledPallet(std::shared_ptr<Pallet> pallet) {
            for (int i = 0; i < packing_pallets.size(); i++) {
                if (packing_pallets[i] == nullptr) {
                    packing_pallets[i] = pallet;
                    return;
                }
            }
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

        int getFreePackingSlots() {
            int free_slots = 0;
            for (int i = 0; i < packing_pallets.size(); i++) {
                if (packing_pallets[i] == nullptr) {
                    free_slots++;
                }
            }
            return free_slots;
        }

        TypeLoads assemblePallet(int work_time, TypeLoads needs, std::string destination) {
            if (getFreeSlots() == disassemble_slots_number) return needs;
            std::map<int, int> needs_map = {{1, needs.type1_load}, {2, needs.type2_load}, {3, needs.type3_load}};

            for (std::shared_ptr<Pallet> dis_pallet : disassemble_pallets) {
                if (needs_map[dis_pallet->getType()] == 0) continue;
                int moved_load = std::min(dis_pallet->getLoad(), needs_map[dis_pallet->getType()]);
                std::shared_ptr<Pallet> asm_pallet = std::make_shared<Pallet>(
                    dis_pallet->getType(),
                    moved_load,
                    destination
                );
                dis_pallet->removeLoad(moved_load);
                putAssembledPallet(asm_pallet);
                needs_map[dis_pallet->getType()] -= moved_load;
                switch (dis_pallet->getType()) {
                    case 1:
                        needs.type1_load -= moved_load;
                        break;
                    case 2:
                        needs.type2_load -= moved_load;
                        break;
                    case 3:
                        needs.type3_load -= moved_load;
                        break;
                }
                return needs;
            }
            return needs;
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


        std::vector<std::shared_ptr<Pallet>> getDisassemblePallets() {
            return disassemble_pallets;
        }
        void removeDisassemblePallet(std::vector<int> indexes) {
            for (int index : indexes){
                disassemble_pallets[index] = nullptr;
            }
        }

        std::vector<std::shared_ptr<Pallet>> getAssembledPallets() {
            return packing_pallets;
        }
        void removeAssembledPallet(std::vector<int> indexes) {
            for (int index : indexes){
                packing_pallets[index] = nullptr;
            }
        }
};

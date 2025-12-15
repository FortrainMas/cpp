#include <vector>
#include <optional>
#include <memory>
#include <mutex>

#include "entities/Pallet.hpp"


using PalletSlots = std::vector<std::shared_ptr<Pallet>>;

class Table {
    private:
        int disassemble_slots_number;
        int packing_slots_number;
        PalletSlots disassemble_pallets;
        PalletSlots packing_pallets;
        std::mutex mutex;
    public:
        Table(int disassemble_slots_number, int packing_slots_number) :
         disassemble_slots_number(disassemble_slots_number), packing_slots_number(packing_slots_number),
         disassemble_pallets(disassemble_slots_number), packing_pallets(packing_slots_number) {};

        std::shared_ptr<Pallet> get_pallet() {
            std::lock_guard<std::mutex> lock(mutex);
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
            std::lock_guard<std::mutex> lock(mutex);

            for(int i = 0; i < disassemble_pallets.size(); i++) {
                if (disassemble_pallets[i] == nullptr) {
                    disassemble_pallets[i] = pallet;
                    return true;
                }
            }
            return false;
        }

        
};

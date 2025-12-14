#include <vector>
#include <optional>
#include <memory>

#include "entities/Pallet.hpp"
#include "accounting_system/PackingZoneAccounting.hpp"

class TableTerminal {
    private:
        std::weak_ptr<TableAccounting> table_accounting;
    public:
        TableTerminal(std::weak_ptr<TableAccounting> table_accounting) : table_accounting(table_accounting) {}
    
        void add_load(std::weak_ptr<Pallet> pallet) {

        }

        void remove_load(std::weak_ptr<Pallet> pallet) {

        }
};

using PalletSlots = std::vector<std::optional<std::shared_ptr<Pallet>>>;

class Table {
    private:
        int disassemble_slots_number;
        int packing_slots_number;
        PalletSlots disassemble_pallets;
        PalletSlots packing_pallets;
        TableTerminal terminal;
    public:
        Table(int disassemble_slots_number, int packing_slots_number) :
         disassemble_slots_number(disassemble_slots_number), packing_slots_number(packing_slots_number),
         disassemble_pallets(disassemble_slots_number), packing_pallets(packing_slots_number) {};
};

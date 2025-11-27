#pragma once

#include <variant>

class ReceivingDockTask {
    private:
        int slot_number;
    public:
        ReceivingDockTask(int slot_number) : slot_number(slot_number) {}
        int getSlotNumber() { return slot_number; }
};

using Task = std::variant<ReceivingDockTask>;


#pragma once

class RecevingDockAccounting {
    private:
        int slots_number;

    public:
        RecevingDockAccounting(int slots_number) : slots_number(slots_number) {}

        int getSlotsNumber() const { return slots_number; }
};

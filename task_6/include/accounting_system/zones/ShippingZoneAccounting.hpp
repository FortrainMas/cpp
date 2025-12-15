class ShippingZoneAccounting {
    private:
        int slots_number;

    public:
        ShippingZoneAccounting(int slots_number) : slots_number(slots_number) {}

        int getSlotsNumber() const { return slots_number; }
};

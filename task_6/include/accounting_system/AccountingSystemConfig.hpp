#pragma once

class AccountingSystemConfig {
    private:
        int receving_dock_slots_number;
        int shipping_zone_slots_number;
        int packing_zone_num_tables;
        int storage_zone_pallet_slots_number;

    public:
        AccountingSystemConfig(
            int receving_dock_slots_number, 
            int shipping_zone_slots_number, 
            int packing_zone_num_tables, 
            int storage_zone_pallet_slots_number
        ) : receving_dock_slots_number(receving_dock_slots_number),
            shipping_zone_slots_number(shipping_zone_slots_number),
            packing_zone_num_tables(packing_zone_num_tables),
            storage_zone_pallet_slots_number(storage_zone_pallet_slots_number) {}

        int getRecevingDockSlotsNumber() const { return receving_dock_slots_number; }
        int getShippingZoneSlotsNumber() const { return shipping_zone_slots_number; }
        int getPackingZoneNumTables() const { return packing_zone_num_tables; }
        int getStorageZonePalletSlotsNumber() const { return storage_zone_pallet_slots_number; }
};

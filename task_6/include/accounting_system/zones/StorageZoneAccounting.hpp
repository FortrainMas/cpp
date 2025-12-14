#include <vector>
#include <memory>
#include <optional>

#include "entities/Pallet.hpp"


#include <vector>

class PalletUpdate {
    private:
        std::weak_ptr<Pallet> pallet;
        int upload;

    public:
        PalletUpdate(std::weak_ptr<Pallet> pallet, int upload) : pallet(pallet), upload(upload) {};
};


class StorageZoneRelocation {
    private:
        std::vector<PalletUpdate> pallet_updates;

    public:
        StorageZoneRelocation(std::vector<PalletUpdate> pallet_updates) : pallet_updates(pallet_updates) {};

        std::vector<PalletUpdate> getPalletUpdates() const { return pallet_updates; }
};




using PalletSlotsAccounting = std::vector<std::optional<std::weak_ptr<Pallet>>>;
class StorageZoneAccounting {
    private:
        int pallet_slots_number;
        PalletSlotsAccounting pallets;

    public:
        StorageZoneAccounting(int pallet_slots_number) : pallet_slots_number(pallet_slots_number), pallets(pallet_slots_number) {};

        std::unique_ptr<StorageZoneRelocation> getRelocation(Pallet pallet); 
};

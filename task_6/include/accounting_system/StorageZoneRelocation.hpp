#include <vector>

class PalletUpdate {
    private:
        int pallet_position;
        int upload;

    public:
        PalletUpdate(int pallet_position, int upload) : pallet_position(pallet_position), upload(upload) {};
};

class StorageZoneRelocation {
    private:
        std::vector<PalletUpdate> pallet_updates;

    public:
        StorageZoneRelocation(std::vector<PalletUpdate> pallet_updates) : pallet_updates(pallet_updates) {};

        std::vector<PalletUpdate> getPalletUpdates() const { return pallet_updates; }
};

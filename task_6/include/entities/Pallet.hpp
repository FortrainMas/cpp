#include <string>

class Pallet {
    private:
        int type;
        int load;
    public:
        Pallet(int type, int load) : type(type), load(load) {};
        int getType() const { return type; }
        int getLoad() const { return load; }
        int getMaxLoad();
        void addLoad(int load);
        void removeLoad(int load);
};

class PackedPallet : public Pallet {
    private:
        std::string destination;
    public:
        PackedPallet(int type, int load, std::string destination) : Pallet(type, load), destination(destination) {};
        std::string getDestination() const { return destination; }
};

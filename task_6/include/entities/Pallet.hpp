#include <string>
#include <optional>

class Pallet {
    private:
        int type;
        int load;
        std::optional<std::string> destination;
        bool ready;
    public:
        Pallet(int type, int load) : type(type), load(load) {};
        Pallet(int type, int load, std::string destination) : type(type), load(load), destination(destination) {};
        int getType() const { return type; }
        int getLoad() const { return load; }
        std::optional<std::string> getDestination() const { return destination; }
        bool isReady() const { return ready; }
        void setReady() { this->ready = true; }

        void addLoad(int load) {this->load += load;}
        void removeLoad(int load) {this->load -= load;}
        
        int getMaxLoad() {
            switch (type)
            {
                case 1:
                    return 100;
                    break;
                case 2:
                    return 30;
                    break;
                case 3:
                    return 4;
                    break;
                default:
                    return -1;
                    break;
            }
        }
};

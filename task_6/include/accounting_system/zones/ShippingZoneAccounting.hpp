#pragma once

#include <memory>
#include <string>
#include <set>
#include <functional>

class ShippingZoneAccounting {
    private:
        int slots_number;
        std::function<std::weak_ptr< std::set<std::string>>()> getDestinationsSave;

    public:
        ShippingZoneAccounting(int slots_number) : slots_number(slots_number) {}

        int getSlotsNumber() const { return slots_number; }

        void registerDestination(std::function<std::weak_ptr< std::set<std::string>>()> getDestinationsSave) 
            { this->getDestinationsSave = getDestinationsSave; }
        std::weak_ptr< std::set<std::string>> getDestinations() const { return getDestinationsSave(); }
};

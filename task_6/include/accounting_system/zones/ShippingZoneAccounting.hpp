#pragma once

#include <memory>
#include <string>
#include <set>

using Destinations = std::set<std::string>;
using DestinationsPtr = std::weak_ptr<Destinations>;
class ShippingZoneAccounting {
    private:
        int slots_number;
        DestinationsPtr destination;

    public:
        ShippingZoneAccounting(int slots_number) : slots_number(slots_number) {}

        int getSlotsNumber() const { return slots_number; }

        void registerDestination(DestinationsPtr destination) { this->destination = destination; }
        DestinationsPtr getDestinations() const { return destination; }
};

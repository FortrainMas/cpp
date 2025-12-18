#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include <functional>
#include <set>

#include "accounting_system/AccountingSystem.hpp"
#include "accounting_system/zones/ShippingZoneAccounting.hpp"
#include "entities/ShippingCar.hpp"

class ShippingCar;
class Pallet;

class ShippingSlot : public std::enable_shared_from_this<ShippingSlot> {
    private:
        std::timed_mutex terminal_mutex;
        std::shared_ptr<ShippingCar> shippingCar;
        std::weak_ptr<AccountingSystem> acc_sys;
        std::function<void(const std::string&)> departure_callback;

    public:
        ShippingSlot(std::function<void(const std::string&)> departure_callback) : departure_callback(departure_callback), shippingCar(nullptr) {}

        std::function<void(const std::string&)> getReleaseCallback() {
            std::weak_ptr<ShippingSlot> weak_this = shared_from_this();
            return [weak_this](const std::string& destination) {
                if (auto slot = weak_this.lock()) {
                    slot->releaseCar(destination);
                }
            };
        }
        void releaseCar(const std::string& destination) {
            shippingCar.reset();
            departure_callback(destination);
        }
        


        std::weak_ptr<ShippingCar> getCar() const { return shippingCar; }
        void acceptCar(std::shared_ptr<ShippingCar> shippingCar) {
            shippingCar->registerCallback(getReleaseCallback());
            this->shippingCar = std::move(shippingCar);
        }
};

class ShippingZone : public std::enable_shared_from_this<ShippingZone>  {
    private:
        std::weak_ptr<AccountingSystem> acc_sys;
        std::vector<std::shared_ptr<ShippingSlot>> slots;
        std::shared_ptr<Destinations> destinations;
        
    public:
        ShippingZone(std::shared_ptr<AccountingSystem> acc_sys) : acc_sys(acc_sys) {
            std::shared_ptr<ShippingZoneAccounting> shipping_zone_accounting = acc_sys->getShippingZoneAccounting().lock();
            
            shipping_zone_accounting->registerDestination(destinations);
            int slots_count = shipping_zone_accounting->getSlotsNumber();
            slots.reserve(slots_count);
            
            for (int i = 0; i < slots_count; ++i) {
                slots.push_back(std::make_shared<ShippingSlot>(
                    [this](const std::string& destination) {
                        this->releaseCar(destination);
                    }
                ));
            }
        }

        void releaseCar(const std::string& destination) {
            destinations->erase(destination);
        }
        
        std::weak_ptr<ShippingSlot> getSlot(std::string destination) const { 
            for(auto slot : slots) {
                if (slot->getCar().lock() != nullptr && slot->getCar().lock()->getDestination() == destination) {
                    return slot;
                }
            }
            return {};
        }
        
        int acceptCar(std::shared_ptr<ShippingCar> shippingCar) {
            for (int i = 0; i < static_cast<int>(slots.size()); i++) {
                if (slots[i]->getCar().lock() == nullptr) {
                    slots[i]->acceptCar(shippingCar);
                    destinations->insert(shippingCar->getDestination());
                    return i;
                }
            }
        }
};

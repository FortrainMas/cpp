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
#include "utils/Logger.hpp"

class ShippingCar;
class Pallet;

class ShippingSlot : public std::enable_shared_from_this<ShippingSlot> {
    private:
        std::shared_ptr<ShippingCar> shippingCar;
        std::weak_ptr<AccountingSystem> acc_sys;
        std::function<void(const std::string&, std::function<void()>)> departure_callback;
        void setCarReleased() { shippingCar.reset(); }

    public:
        ShippingSlot(std::function<void(const std::string&, std::function<void()>)> departure_callback) : departure_callback(departure_callback), shippingCar(nullptr) {}

        std::function<void(const std::string&)> getReleaseCallback() {
            std::weak_ptr<ShippingSlot> weak_this = shared_from_this();
            return [weak_this](const std::string& destination) {
                if (auto slot = weak_this.lock()) {
                    slot->releaseCar(destination);
                }
            };
        }
        void releaseCar(const std::string& destination) {
            departure_callback(destination, [this]() { setCarReleased(); });
        }
        


        std::weak_ptr<ShippingCar> getCar() const { return shippingCar; }
        void acceptCar(std::shared_ptr<ShippingCar> shippingCar) {
            shippingCar->registerCallback(getReleaseCallback());
            this->shippingCar = std::move(shippingCar);
        }
};

class ShippingZone : public std::enable_shared_from_this<ShippingZone>  {
    private:
        std::mutex mutex;
        std::weak_ptr<AccountingSystem> acc_sys;
        std::vector<std::shared_ptr<ShippingSlot>> slots;
        std::shared_ptr<std::set<std::string>> destinations;
        
    public:
        ShippingZone(std::shared_ptr<AccountingSystem> acc_sys) : acc_sys(acc_sys), destinations(std::make_shared<std::set<std::string>>()) {
            std::shared_ptr<ShippingZoneAccounting> shipping_zone_accounting = acc_sys->getShippingZoneAccounting().lock();
            
            shipping_zone_accounting->registerDestination([this](){std::lock_guard<std::mutex> lock(mutex); return destinations;});
            int slots_count = shipping_zone_accounting->getSlotsNumber();
            slots.reserve(slots_count);
            
            for (int i = 0; i < slots_count; ++i) {
                slots.push_back(std::make_shared<ShippingSlot>(
                    [this](const std::string& destination, std::function<void()> callback) {
                        this->releaseCar(destination, callback);
                    }
                ));
            }
        }

        void releaseCar(const std::string& destination, std::function<void()> callback) {
            std::lock_guard<std::mutex> lock(mutex);
            destinations->erase(destination);
            Logger::log("Releasing shipping car from slot to " + destination);
            callback();
        }
        
        std::weak_ptr<ShippingSlot> getSlot(std::string destination) { 
            std::lock_guard<std::mutex> lock(mutex);
            for(auto slot : slots) {
                if (slot->getCar().lock() != nullptr && slot->getCar().lock()->getDestination() == destination) {
                    return slot;
                }
            }
            return {};
        }
        
        int acceptCar(std::shared_ptr<ShippingCar> shippingCar) {
            std::lock_guard<std::mutex> lock(mutex);
            Logger::log("Accepting shipping car on slot");
            for (int i = 0; i < static_cast<int>(slots.size()); i++) {
                if (slots[i]->getCar().lock() == nullptr) {
                    slots[i]->acceptCar(shippingCar);
                    destinations->insert(shippingCar->getDestination());
                    return i;
                }
            }
            return -1;
        }

        std::shared_ptr<std::set<std::string>> getDestinations() {
            std::lock_guard<std::mutex> lock(mutex);
            return std::make_shared<std::set<std::string>>(*destinations);
        }

};

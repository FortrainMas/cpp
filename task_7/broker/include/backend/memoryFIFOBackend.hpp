#pragma once

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <mutex>
#include <chrono>
#include <optional>
#include <list>
#include <vector>
#include <cstdlib>

#include "backend/backend.hpp"
#include "message.hpp"
#include "utils/Logger.hpp"

class MemoryFIFOBackend : public Backend {
private:
    int message_ttl_;
    std::mutex mutex_;

    std::list<Message> queue;
    std::unordered_map<Client, std::list<Message>::iterator> client_position;

    auto now() const { return std::chrono::system_clock::now(); }

public:
    MemoryFIFOBackend(int message_ttl) : message_ttl_(message_ttl) {}

    void registerSubscriber(Client client) override {
        std::lock_guard<std::mutex> lock(mutex_);
        client_position[client] = queue.begin();
    }

    void unregisterSubscriber(Client client) override {
        std::lock_guard<std::mutex> lock(mutex_);
        client_position.erase(client);
    }

    void pruneQueue() override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now_time = now();
        auto threshold_time = now_time - std::chrono::seconds(message_ttl_);

        for (auto it = queue.begin(); it != queue.end(); ) {
            Message& msg = *it;

            if (!msg.validateAckTimestamp()) {
                auto msg_copy = std::move(*it);
                auto msg_it = it++;
                queue.erase(msg_it);
                queue.push_back(std::move(msg_copy));
                        
                for (auto& [client, pos] : client_position) {
                    if (pos == msg_it) {
                        pos = it;
                    }
                }
                continue;
            }

            if (msg.getTimestamp() <= threshold_time) {
                auto msg_it = it++;
                queue.erase(msg_it);
                
                for (auto& [client, pos] : client_position) {
                    if (pos == msg_it) {
                        pos = queue.begin();
                    }
                }
            } else {
                it++;
            }
        }
    }

    bool addMessage(Message message) override {
        std::lock_guard<std::mutex> lock(mutex_);
        Logger::log("In queue put message: " + message.getPayload());
        queue.push_back(std::move(message));
        
        auto new_msg_it = std::prev(queue.end());
    
        for (auto& [client, pos] : client_position) {
            if (pos == queue.end()) {
                pos = new_msg_it;
            }
        }
        return true;
    }

    std::optional<Message> getMessage(Client client) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it_client = client_position.find(client);
        if (it_client == client_position.end()) return std::nullopt;

        if (queue.empty()) return std::nullopt;

        auto& client_it = it_client->second;
        
        if (client_it == queue.end()) {
            return std::nullopt;
        }

        for (size_t i = 0; i < queue.size(); i++) {
            if (client_it == queue.end()) {
                client_it = queue.begin();
            }

            Message& msg = *client_it;

            bool no_ack_needed = (client.qos() == ClientQoS::NO_ACK) || (msg.getAckTimestamp() == std::nullopt);
            if (no_ack_needed) {
                if (client.qos() == ClientQoS::ACK) {
                    msg.setAckTimestamp(client.ackTTL());
                }
                
                auto result = std::move(msg);
                client_it++;
                Logger::log("Client" + client.id() + " took message: " + result.getPayload());
                return result;
            }
            
            client_it++;
        }

        Logger::log("Client failed to acquire message");
        return std::nullopt;
    }

    void ackMessage(Client client, const std::string key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it_client = client_position.find(client);
        if (it_client == client_position.end()) return;

        auto it_msg = std::find_if(queue.begin(), queue.end(),
            [&key](const Message& msg) { return msg.getId() == key; });
        
        if (it_msg != queue.end()) {
            if (it_client->second == it_msg) {
                it_client->second = std::next(it_msg);
                if (it_client->second == queue.end()) {
                    it_client->second = queue.begin();
                }
            }
            
            queue.erase(it_msg);
        }
    }
};

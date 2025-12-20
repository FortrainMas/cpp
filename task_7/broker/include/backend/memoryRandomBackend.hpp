#pragma once

#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <optional>
#include <vector>
#include <cstdlib>

#include "backend/backend.hpp"
#include "message.hpp"

class MemoryRandomBackend : public Backend {
private:
    int message_ttl_;
    std::mutex mutex_;

    std::unordered_map<std::string, Message> queue;
    std::unordered_map<Client, std::unordered_set<std::string>> client_sent;

    auto now() const { return std::chrono::system_clock::now(); }

public:
    MemoryRandomBackend(int message_ttl) : message_ttl_(message_ttl) {}

    void registerSubscriber(Client client) override {
        std::lock_guard<std::mutex> lock(mutex_);
        client_sent[client] = {};
    }

    void unregisterSubscriber(Client client) override {
        std::lock_guard<std::mutex> lock(mutex_);
        client_sent.erase(client);
    }

    void pruneQueue() override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now_time = now();

        for (auto it = queue.begin(); it != queue.end(); ) {
            Message& msg = it->second;

            msg.validateAckTimestamp(); 

            if (msg.getTimestamp() + std::chrono::seconds(message_ttl_) <= now_time) {
                for (auto& [client, sent_set] : client_sent) {
                    sent_set.erase(it->first);
                }
                it = queue.erase(it);
            } else {
                it++;
            }
        }
    }

    bool addMessage(Message message) override {
        std::lock_guard<std::mutex> lock(mutex_);
        queue[message.getId()] = std::move(message);
        return true;
    }

    std::optional<Message> getMessage(Client client) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it_client = client_sent.find(client);
        if (it_client == client_sent.end()) return std::nullopt;

        if (queue.empty()) return std::nullopt;

        std::vector<std::reference_wrapper<Message>> candidates;
        for (auto& [id, msg] : queue) {
            bool not_sent = it_client->second.count(id) == 0;
            bool no_ack_needed = (client.qos() == ClientQoS::NO_ACK) || (msg.getAckTimestamp() == std::nullopt);
            if (not_sent && no_ack_needed) {
                candidates.push_back(msg);
            }
        }

        if (candidates.empty()) return std::nullopt;

        Message& msg = candidates[rand() % candidates.size()].get();

        if (client.qos() == ClientQoS::ACK) {
            it_client->second.insert(msg.getId());
            msg.setAckTimestamp(client.ackTTL());
        }

        return msg;
    }

    void ackMessage(Client client, const std::string key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it_client = client_sent.find(client);
        if (it_client == client_sent.end()) return;

        auto it_msg = queue.find(key);
        if (it_msg != queue.end()) {
            queue.erase(it_msg); 
        }

        for (auto& [c, sent_set] : client_sent) {
            sent_set.erase(key);
        }
    }
};

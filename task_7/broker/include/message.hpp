#pragma once

#include <chrono>
#include <string>
#include <optional>

class Message {
    private:
        std::string id_;
        std::string payload_;
        std::chrono::time_point<std::chrono::system_clock> timestamp_;
        std::optional<std::chrono::time_point<std::chrono::system_clock>> ack_timestamp_;
        int ack_ttl;

    public:
        Message() = default;
        Message(std::string payload) : payload_(payload), timestamp_(std::chrono::system_clock::now()) {}
        Message(std::string id, std::string payload) : id_(id), payload_(payload), 
        timestamp_(std::chrono::system_clock::now()) {}

        void setId(std::string id) { id_ = id; }

        void setAckTimestamp(int ttl) {
            ack_ttl = ttl;
            ack_timestamp_ = std::chrono::system_clock::now();
        }

        void removeAckTimestamp() {
            ack_timestamp_ = std::nullopt;
        }

        bool validateAckTimestamp()  {
            if (ack_timestamp_ != std::nullopt) {
                if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - ack_timestamp_.value()).count() > ack_ttl) {
                    removeAckTimestamp();
                    return false;
                }
            }
            return true;
        }

        std::string getId() const { return id_; }
        std::string getPayload() const { return payload_; }
        std::chrono::time_point<std::chrono::system_clock> getTimestamp() const { return timestamp_; }
        std::optional<std::chrono::time_point<std::chrono::system_clock>> getAckTimestamp() const { return ack_timestamp_; }
        void setTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp) { timestamp_ = timestamp; }

        bool operator==(const Message& other) const { return id_ == other.getId(); }
};

namespace std {
    template <>
    struct hash<Message> {
        size_t operator()(const Message& m) const {
            return hash<std::string>()(m.getId());
        }
    };
}
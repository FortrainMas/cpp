#pragma once

#include <string>

enum class ClientQoS {
    ACK,
    NO_ACK
};

class Client {
    private:
        std::string id_;
        ClientQoS qos_;
        int ack_ttl_;
    public:
        Client() = default;
        Client(std::string id, ClientQoS qos) : id_(id), qos_(qos) {}
        Client(std::string id, ClientQoS qos, int ack_ttl) : id_(id), qos_(qos), ack_ttl_(ack_ttl) {}
        int ackTTL() const { return ack_ttl_; }
        std::string id() const { return id_; }
        ClientQoS qos() const { return qos_; }
        bool operator==(const Client& other) const { return id_ == other.id(); }
};

namespace std {
    template<>
    struct hash<Client> {
        size_t operator()(const Client& c) const { return hash<std::string>()(c.id()); }
    };
}
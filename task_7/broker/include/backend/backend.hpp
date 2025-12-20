#pragma once

#include <string>
#include <optional>

#include "client.hpp"
#include "message.hpp"
#include "utils/Logger.hpp"

class Backend {
    public:
        void virtual registerSubscriber(Client client) {}
        void virtual unregisterSubscriber(Client client) {}
        void virtual pruneQueue() { Logger::log("Prunning of virtual backend"); }

        bool virtual addMessage(Message message) {return false;}
        void virtual ackMessage(Client client, std::string key) {}
        std::optional<Message> virtual getMessage(Client client) { return std::nullopt; }

        virtual ~Backend() = default;
};

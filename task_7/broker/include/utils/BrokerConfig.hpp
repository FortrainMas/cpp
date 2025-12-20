#pragma once

#include <yaml-cpp/yaml.h>
#include <string>
#include <stdexcept>

#include <string>
#include "queue.hpp"

class QueueConfig {
    public:
        std::string name;
        QueueType type;
        QueueDurability durability;
        int ttl;
};


class BrokerConfig {
    public:
        std::vector<QueueConfig> queues;

        static BrokerConfig loadFromFile(const std::string& path) {
            BrokerConfig cfg;
            cfg = YAML::LoadFile(path).as<BrokerConfig>();
            Logger::log("Config loaded successfully");
            return cfg;
        }
};


inline QueueType parseQueueType(const std::string& s) {
    if (s == "fifo")   return QueueType::FIFO;
    if (s == "random") return QueueType::RANDOM;
    throw std::runtime_error("Unknown QueueType: " + s);
}

inline QueueDurability parseQueueDurability(const std::string& s) {
    if (s == "disk")   return QueueDurability::DISK;
    if (s == "memory") return QueueDurability::MEMORY;
    throw std::runtime_error("Unknown QueueDurability: " + s);
}


namespace YAML {

    template<>
    struct convert<QueueConfig> {
        static bool decode(const Node& node, QueueConfig& cfg) {
            if (!node.IsMap())
                return false;

            if (!node["name"] || !node["type"] || !node["durability"] || !node["ttl"])
                throw std::runtime_error("QueueConfig: missing required field");

            cfg.name = node["name"].as<std::string>();
            cfg.type = parseQueueType(node["type"].as<std::string>());
            cfg.durability = parseQueueDurability(node["durability"].as<std::string>());
            cfg.ttl = node["ttl"].as<int>();

            if (cfg.ttl <= 0)
                throw std::runtime_error("QueueConfig.ttl must be > 0");

            return true;
        }
    };

    template<>
    struct convert<BrokerConfig> {
        static bool decode(const Node& node, BrokerConfig& cfg) {
            if (!node.IsMap())
                return false;

            if (!node["queues"] || !node["queues"].IsSequence())
                throw std::runtime_error("BrokerConfig: 'queues' must be a sequence");

            cfg.queues.clear();
            for (const auto& q : node["queues"]) {
                cfg.queues.push_back(q.as<QueueConfig>());
            }

            if (cfg.queues.empty())
                throw std::runtime_error("BrokerConfig: queues list is empty");

            return true;
        }
    };

}


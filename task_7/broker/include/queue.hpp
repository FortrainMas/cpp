#pragma once

#include <string>
#include <iostream>

#include "message.hpp"
#include "client.hpp"
#include "backend/backend.hpp"
#include "backend/memoryRandomBackend.hpp"
#include "backend/memoryFIFOBackend.hpp"
#include "backend/diskFIFOBackend.hpp"

#include "utils/Logger.hpp"

enum class QueueType {
    FIFO,
    RANDOM
};

enum class QueueDurability {
    DISK,
    MEMORY
};

class Queue {
    private:
        std::string name_;
        QueueType type_;
        QueueDurability durability_;
        int message_ttl_;
        std::shared_ptr<Backend> backend;
        
        int message_last_id_ = 0;
        std::mutex mutex_;


        int incrementMessageId() {
            std::lock_guard<std::mutex> lock(mutex_);
            message_last_id_ += 1;
            return message_last_id_;
        }

    public:
        Queue(std::string name, QueueType type, QueueDurability durability, int message_ttl) :
            name_(name), type_(type), durability_(durability), message_ttl_(message_ttl) {
                if (durability_ == QueueDurability::MEMORY) {
                    if (type_ == QueueType::RANDOM) backend = std::make_shared<MemoryRandomBackend>(message_ttl);
                    else backend = std::make_shared<MemoryFIFOBackend>(message_ttl);
                } else {
                    #include <filesystem>
                    namespace fs = std::filesystem;
                    fs::path cwd = fs::current_path();
                    backend = std::make_shared<DiskFIFOBackend>(cwd, message_ttl);
                }

                
                if (backend == nullptr) {
                    Logger::log("NO BACKEND");
                }
            }

        void registerSubscriber(Client client) {
            backend->registerSubscriber(client);
        }
        void unregisterSubscriber(Client client) {
            backend->unregisterSubscriber(client);
        }
        void pruneQueue() {
            backend->pruneQueue();
        }

        std::string getName() const { return name_; }
        bool addMessage(Message message) {
            message.setId(std::to_string(incrementMessageId()));
            return backend->addMessage(message);
        }
        Message getMessage(Client client) {
            return backend->getMessage(client).value();
        }

        void ackMessage(Client client, std::string id) {
            backend->ackMessage(client, id);
        }
};

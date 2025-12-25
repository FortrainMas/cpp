#pragma once

#include <filesystem>
#include <fstream>
#include <mutex>
#include <optional>
#include <chrono>
#include <vector>
#include <algorithm>

#include <nlohmann/json.hpp>

#include "backend/backend.hpp"
#include "message.hpp"
#include "utils/Logger.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

class DiskFIFOBackend : public Backend {
private:
    fs::path root_;
    int message_ttl_;
    std::mutex mutex_;

    static auto now() {
        return std::chrono::system_clock::now();
    }

    static std::string makeFilename(const Message& msg) {
        auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
            msg.getTimestamp().time_since_epoch()
        ).count();

        return std::to_string(ts) + "_" + msg.getId() + ".json";
    }

    static void writeMessageAtomic(const fs::path& path, const Message& msg) {
        json j;
        j["id"] = msg.getId();
        j["payload"] = msg.getPayload();
        j["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
            msg.getTimestamp().time_since_epoch()
        ).count();

        fs::path tmp = path;
        tmp += ".tmp";

        {
            std::ofstream out(tmp, std::ios::trunc);
            out << j.dump();
        }

        fs::rename(tmp, path);
    }

    static std::optional<Message> readMessage(const fs::path& path) {
        try {
            std::ifstream in(path);
            if (!in.is_open()) return std::nullopt;

            json j;
            in >> j;

            Message msg(j["id"], j["payload"]);

            auto ts = std::chrono::system_clock::time_point{
                std::chrono::seconds(j["timestamp"])
            };

            msg = Message(j["id"], j["payload"]);

            msg.setTimestamp(ts);

            return msg;
        } catch (...) {
            return std::nullopt;
        }
    }

    std::vector<fs::path> listMessages() const {
        std::vector<fs::path> files;
        for (auto& p : fs::directory_iterator(root_)) {
            if (p.path().extension() == ".json") {
                files.push_back(p.path());
            }
        }
        std::sort(files.begin(), files.end());
        return files;
    }

public:
    DiskFIFOBackend(fs::path root, int message_ttl)
        : root_(std::move(root)), message_ttl_(message_ttl) {
        fs::create_directories(root_);
    }

    void registerSubscriber(Client) override {}
    void unregisterSubscriber(Client) override {}

    bool addMessage(Message message) override {
        std::lock_guard<std::mutex> lock(mutex_);

        if (message.getId().empty()) return false;

        fs::path path = root_ / makeFilename(message);
        writeMessageAtomic(path, message);

        Logger::log("DiskFIFO: put message " + message.getPayload());
        return true;
    }

    std::optional<Message> getMessage(Client client) override {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& path : listMessages()) {
            auto opt = readMessage(path);
            if (!opt) continue;

            Message msg = *opt;

            if (client.qos() == ClientQoS::ACK) {
                msg.setAckTimestamp(client.ackTTL());
            } else {
                fs::remove(path);
            }

            Logger::log("DiskFIFO: client took message " + msg.getPayload());
            return msg;
        }

        return std::nullopt;
    }

    void ackMessage(Client, const std::string key) override {
        std::lock_guard<std::mutex> lock(mutex_);

        for (auto& path : listMessages()) {
            if (path.filename().string().find(key) != std::string::npos) {
                fs::remove(path);
                return;
            }
        }
    }

    void pruneQueue() override {
        std::lock_guard<std::mutex> lock(mutex_);

        auto limit = now() - std::chrono::seconds(message_ttl_);

        for (auto& path : listMessages()) {
            auto opt = readMessage(path);
            if (!opt) {
                fs::remove(path);
                continue;
            }

            if (opt->getTimestamp() <= limit) {
                fs::remove(path);
            }
        }
    }
};

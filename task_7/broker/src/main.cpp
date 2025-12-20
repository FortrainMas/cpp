#include <iostream>
#include <vector>
#include <memory>
#include <yaml-cpp/yaml.h>

#include "zmq.hpp"
#include "queue.hpp"
#include "client.hpp"
#include "broker.hpp"
#include "utils/BrokerConfig.hpp"
#include "utils/Logger.hpp"
#include "mqserver.hpp"


int main(int argc, char* argv[]) {
    Logger::init();
    if (argc < 2) {
        Logger::log("No config file provided");
        return 1;
    }
    
    BrokerConfig cfg = BrokerConfig::loadFromFile(argv[1]);
    Broker broker(cfg);
    Logger::log("Broker created");

    broker.start();
    Logger::log("Logger started");

    MQServer server(broker);
    Logger::log("Starting server");
    server.run();

    std::this_thread::sleep_for(std::chrono::seconds(60));
    Logger::shutdown();
}

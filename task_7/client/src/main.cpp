#include <zmq.hpp>
#include <iostream>
#include <thread>
#include <chrono>

#include "Client.hpp"


int main() {
    zmq::context_t ctx{1};

    DealerClient client(
        ctx,
        "tcp://127.0.0.1:5555",
        "client-1"
    );

    std::cout << "Dealer started\n";

    client.queueRegister("orders", QoS::NO_ACK);

    for (int i = 0; i < 5; ++i) {
        client.queueSend("orders", SendQoS::NO_ACK, "Hello" + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    for(int i = 0; i < 5; i++) {
        client.queueGet("orders");
    }

    for(int i = 0; i < 5; i++) {
        client.queueSend("orders", SendQoS::ACK, "Hello" + std::to_string(i));
    }

    client.queueRegister("metrics", QoS::ACK);
    for (int i = 0; i < 5; ++i) {
        client.queueSend("metrics", SendQoS::ACK, "Hello" + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    for (int i = 0; i < 5; i++) {
        Message message = client.queueGet("metrics");
        client.queueAck("metrics", message.id);
    }

    std::cout << "done\n";
    return 0;
}

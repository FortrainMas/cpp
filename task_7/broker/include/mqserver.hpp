#include <zmq.hpp>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>

#include "broker.hpp"


using json = nlohmann::json;
class MQServer {
    private:
        Broker& broker_;

        std::map<std::pair<std::string, std::string>, Client> clients_;

        std::vector<zmq::message_t> recvMultipart(zmq::socket_t& sock) {
            std::vector<zmq::message_t> parts;
            while (true) {
                zmq::message_t part;
                sock.recv(part);
                parts.push_back(std::move(part));
                if (!sock.get(zmq::sockopt::rcvmore))
                    break;
            }
            return parts;
        }

        void response(zmq::socket_t& router, std::string identity, std::string payload) {
            router.send(zmq::buffer(identity), zmq::send_flags::sndmore);
            router.send(zmq::message_t{}, zmq::send_flags::sndmore);
            router.send(zmq::buffer(payload));
        }

    public:
        MQServer(Broker& broker) : broker_(broker), clients_() {}
        void run() {
            zmq::context_t ctx{1};
            zmq::socket_t router(ctx, zmq::socket_type::router);

            router.set(zmq::sockopt::linger, 0);
            router.bind("tcp://*:5555");
            Logger::log("Binded successfully");

            while (true) {
                auto msg = recvMultipart(router);
                Logger::log("Received message "+ msg.size());

                if (msg.size() < 3) {
                    std::cerr << "invalid message\n";
                    continue;
                }

                std::string identity(
                    static_cast<char*>(msg[0].data()),
                    msg[0].size()
                );

                std::string command(
                    static_cast<char*>(msg[2].data()),
                    msg[2].size()
                );

                Logger::log("Found command: " + command);

                if (command == "REGISTER") {
                    if (msg.size() < 5) {
                        std::cerr << "REGISTER without queue\n";
                        response(router, identity, "BROKEN RESPONSE");
                        continue;
                    }

                    std::string queue_name(
                        static_cast<char*>(msg[3].data()),
                        msg[3].size()
                    );
                    
                    std::string acc(
                        static_cast<char*>(msg[4].data()),
                        msg[4].size()
                    );

                    std::shared_ptr<Queue> queue = broker_.getQueue(queue_name);
                    if (queue == nullptr) {
                        std::cerr << "queue " << queue_name << " does not exist\n";
                        response(router, identity, "NO SUCH QUEUE");
                        continue;
                    }

                    if (acc == "NO_ACK") {
                        clients_[{identity, queue_name}] = Client(identity, ClientQoS::NO_ACK);
                        broker_.getQueue(queue_name)->registerSubscriber(clients_[{identity, queue_name}]);
                    }
                    else {
                        std::string ttl(
                            static_cast<char*>(msg[5].data()),
                            msg[5].size()
                        );
                        clients_[{identity, queue_name}] = Client(identity, ClientQoS::ACK);
                        broker_.getQueue(queue_name)->registerSubscriber(clients_[{identity, queue_name}]);
                    }



                    Logger::log("Client " + identity + " registered to queue " + queue_name);
                    response(router, identity, "OK");
                }

                else if (command == "SEND") {
                    Logger::log("SEND");
                    if (msg.size() < 6) {
                        Logger::log("SEND without payload\n");
                        response(router, identity, "BROKEN RESPONSE");
                        continue;
                    }

                    std::string queue(
                        static_cast<char*>(msg[3].data()),
                        msg[3].size()
                    );

                    std::string ack(
                        static_cast<char*>(msg[4].data()),
                        msg[4].size()
                    );

                    Logger::log("Client " + identity + " sent message to queue " + queue);

                    std::string payload(
                        static_cast<char*>(msg[5].data()),
                        msg[5].size()
                    );
                    
                    if (broker_.getQueue(queue) == nullptr) {
                        std::cerr << "queue " << queue << " does not exist\n";
                        response(router, identity, "NO SUCH QUEUE");
                        continue;
                    }

                    bool result = broker_.getQueue(queue)->addMessage(Message(identity, payload));

                    if(ack == "ACK") {
                        response(router, identity, "OK");
                    }
                }

                else if (command == "GET") {
                    json j;
                    j["status"] = "200";
                    j["id"] = "-1";
                    j["payload"] = "";

                    if (msg.size() < 4) {
                        std::cerr << "SEND without payload\n";
                        j["status"] = "400: SEND without payload";
                        response(router, identity, j.dump());
                        continue;
                    }

                    std::string queue(
                        static_cast<char*>(msg[3].data()),
                        msg[3].size()
                    );

                    if (broker_.getQueue(queue) == nullptr) {
                        std::cerr << "queue " << queue << " does not exist\n";
                        j["status"] = "404: No such queue";
                        response(router, identity, j.dump());
                        continue;
                    }


                    Client client = clients_[{identity, queue}];
                    Message message = broker_.getQueue(queue)->getMessage(client);

                    j["id"] = message.getId();
                    j["payload"] = message.getPayload();
                    response(router, identity, j.dump());
                }

                else if(command == "ACK") {
                    json j;
                    j["status"] = "200";
                    j["id"] = "-1";
                    j["payload"] = "";

                    if (msg.size() < 4) {
                        std::cerr << "ACK without id\n";
                        j["status"] = "400: ACK without id";
                        response(router, identity, j.dump());
                        continue;
                    }

                    std::string queue(
                        static_cast<char*>(msg[3].data()),
                        msg[3].size()
                    );

                    if (broker_.getQueue(queue) == nullptr) {
                        std::cerr << "queue " << queue << " does not exist\n";
                        j["status"] = "404: No such queue";
                        response(router, identity, j.dump());
                        continue;
                    }

                    std::string id(
                        static_cast<char*>(msg[4].data()),
                        msg[4].size()
                    );

                    Client client = clients_[{identity, queue}];
                    broker_.getQueue(queue)->ackMessage(client, id);
                    Logger::log("In queue " + queue + " acked message with id " + id);

                    j["status"] = "200";
                    j["id"] = id;
                    j["payload"] = "OK";
                    response(router, identity, j.dump());
                }
            }
        }
};  

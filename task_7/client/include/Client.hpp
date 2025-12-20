#include <zmq.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>

enum class QoS {
    ACK,
    NO_ACK
};

enum class SendQoS {
    ACK,
    NO_ACK
};

class Message {
public:
    std::string id;
    std::string payload;
    Message(std::string id, std::string payload) : id(id), payload(payload) {}
};

using json = nlohmann::json;
class DealerClient {
    private:
        zmq::socket_t sock_;

        void send(const std::vector<std::string>& frames) {
            if (frames.empty()) return;

            sock_.send(zmq::message_t{}, zmq::send_flags::sndmore);

            for (size_t i = 0; i < frames.size(); ++i) {
                sock_.send(
                    zmq::buffer(frames[i]),
                    i + 1 < frames.size()
                        ? zmq::send_flags::sndmore
                        : zmq::send_flags::none
                );
            }
        }

        std::vector<std::string> recv() {
            std::vector<std::string> res;
            while (true) {
                zmq::message_t msg;
                sock_.recv(msg);
                res.emplace_back(
                    static_cast<char*>(msg.data()),
                    msg.size()
                );
                if (!sock_.get(zmq::sockopt::rcvmore))
                    break;
            }
            return res;
        }


    public:
        DealerClient(zmq::context_t& ctx,
                     std::string endpoint,
                     std::string id)
            : sock_(ctx, zmq::socket_type::dealer)
        {
            sock_.set(zmq::sockopt::linger, 0);
            sock_.set(zmq::sockopt::routing_id, id);
            sock_.connect(endpoint);
        }

        std::vector<std::string> queueRegister(std::string queue_name, QoS qos, int ttl=-1) {
            if (qos == QoS::ACK) {
                send({"REGISTER", queue_name, "ACK", std::to_string(ttl)});
            }
            else {
                send({"REGISTER", queue_name, "NO_ACK"});
            }
            std::vector<std::string> response = recv();
            for(int i = 0; i < response.size(); i++){
                std::cout << response[i] << "\n";
            }
            return response;
        }

        std::vector<std::string> queueSend(std::string queue_name, SendQoS qos, std::string payload) {
            send({"SEND", queue_name, qos == SendQoS::ACK ? "ACK" : "NO_ACK", payload});

            if (qos == SendQoS::ACK) {
                std::vector<std::string> response = recv();
                for(int i = 0; i < response.size(); i++){
                    std::cout << response[i] << "\n";
                }
                return response;
            }

            return {};
        }

        Message queueGet(std::string queue_name) {
            send({"GET", queue_name});
            std::vector<std::string> response = recv();
            std::string resp;
            for(int i = 0; i < response.size(); i++){
                std::cout << response[i] << "\n";
                resp+=response[i];
            }

            json j = json::parse(resp);
            if (j["status"].get<std::string>() == "200") {
                return Message(j["id"].get<std::string>(), j["payload"].get<std::string>());
            }

            return Message("-1", "");
        }

        std::vector<std::string> queueAck(std::string queue_name, std::string message_id) {
            send({"ACK", queue_name, message_id});
            std::vector<std::string> response = recv();
            for(int i = 0; i < response.size(); i++){
                std::cout << response[i] << "\n";
            }
            return response;
        }
};

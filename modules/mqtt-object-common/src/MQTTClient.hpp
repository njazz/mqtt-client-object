#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

// paho C++ library:
#include "mqtt/async_client.h"

//
using MQTTCallback = std::function<void(const std::string&)>;
using MQTTMsgCallback = std::function<void(const std::string&, const std::string&)>;

//
class MQTTClient : public mqtt::callback {
    std::unique_ptr<mqtt::async_client> mqttClient;
    mqtt::create_options options = mqtt::create_options();
    mqtt::connect_options connectOptions = mqtt::connect_options();

    std::string lastError = "";

    MQTTCallback onConnected = [](const std::string&) {};
    MQTTCallback onConnectionLost = [](const std::string&) {};
    MQTTMsgCallback onMessage = [](const std::string&, const std::string&) {};

    // mqtt::callback
    virtual void connected(const mqtt::string& v) override final { onConnected(v); }
    virtual void connection_lost(const mqtt::string& v) override final { onConnectionLost(v); }
    virtual void delivery_complete(mqtt::delivery_token_ptr /*tok*/) override final { } ///< yet unused
    virtual void message_arrived(mqtt::const_message_ptr /*msg*/) override final;

public:
    ~MQTTClient();

    bool connect(const std::string& host, const unsigned int& port, const std::string& clientName, const std::string& username = "", const std::string& password = ""); ///< host, port
    bool subscribe(const std::string&);
    bool publish(const std::string& k, const std::string& v);

    void unsubscribe(const std::string&);
    void disconnect();

    void setOnConnected(const MQTTCallback& v) { onConnected = v; }
    void setOnConnectionLost(const MQTTCallback& v) { onConnectionLost = v; }
    void setOnMessage(const MQTTMsgCallback& v) { onMessage = v; }

    std::string getLastError() { return lastError; }
};

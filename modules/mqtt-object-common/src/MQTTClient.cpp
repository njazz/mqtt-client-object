#include "mqtt/async_client.h"

#include "MQTTClient.hpp"

// paho C++ library:
#include "mqtt/async_client.h"

// receiver

class MQTTClientImplementation: public mqtt::callback {
    MQTTClient& _obj;
public:
    mqtt::create_options options = mqtt::create_options();
    mqtt::connect_options connectOptions = mqtt::connect_options();
    std::unique_ptr<mqtt::async_client> mqttClient;

    MQTTClientImplementation(MQTTClient& obj): _obj(obj){}

    virtual void connected(const mqtt::string& v) override final { _obj.onConnected(v); }
    virtual void connection_lost(const mqtt::string& v) override final { _obj.onConnectionLost(v); }
    virtual void delivery_complete(mqtt::delivery_token_ptr /*tok*/) override final { } ///< yet unused
    virtual void message_arrived(mqtt::const_message_ptr msg) override final { _obj.onMessage(msg->get_topic(), msg->get_payload_str());}
};

// ---
//void MQTTClient::message_arrived(mqtt::const_message_ptr msg)
//{
//    onMessage(msg->get_topic(), msg->get_payload_str());
//}

// ---

MQTTClient::MQTTClient(){
    implementation.reset(new MQTTClientImplementation(*this));
}

MQTTClient::~MQTTClient(){
}

bool MQTTClient::connect(const std::string& host, const unsigned int& port, const std::string& clientName, const std::string& userName, const std::string& password)
{
    const std::string uri = "tcp://" + host + ":" + std::to_string(port);

    try {
        implementation->options = mqtt::create_options();

        implementation->mqttClient.reset(new mqtt::async_client(uri, clientName, implementation->options)); // , mqtt::create_options(3)
        implementation->mqttClient->set_callback(*implementation);

        implementation->connectOptions = mqtt::connect_options();
        implementation->connectOptions.set_keep_alive_interval(20);
        implementation->connectOptions.set_clean_session(true);

        if (!userName.empty() && !password.empty()) {
            implementation->connectOptions.set_user_name(userName);
            implementation->connectOptions.set_password(password);
        }

        implementation->mqttClient->connect(implementation->connectOptions);

    } catch (std::exception& e) {
        lastError = e.what();
        return false;
    }

    return true;
}

bool MQTTClient::subscribe(const std::string& topic)
{
    if (!implementation->mqttClient)
        return false;

    try {
        implementation->mqttClient->subscribe(topic, 1);
    } catch (std::exception& e) {
        lastError = e.what();
        return false;
    }

    return true;
}

void MQTTClient::unsubscribe(const std::string& topic)
{
    if (!implementation->mqttClient)
        return;

    try {
        implementation->mqttClient->unsubscribe(topic);
    } catch (std::exception& e) {
        lastError = e.what();
        return;
    }
}

bool MQTTClient::publish(const std::string& k, const std::string& v)
{
    if (!implementation->mqttClient)
        return false;

    try {
        implementation->mqttClient->publish(k, v.data(), v.size());
    } catch (std::exception& e) {
        lastError = e.what();
        return false;
    }

    return true;
}

void MQTTClient::disconnect()
{
    if (!implementation->mqttClient)
        return;

    try {
        implementation->mqttClient->disconnect();
    }catch (std::exception& e) {
        lastError = e.what();
    }

    implementation->mqttClient.reset();
}

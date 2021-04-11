#include "MQTTClient.hpp"

// ---
void MQTTClient::message_arrived(mqtt::const_message_ptr msg)
{
    auto k = msg->get_topic();
    if (callbacks.find(k) == callbacks.end())
        return;

    callbacks[k](msg->get_payload_str());
}

// ---

bool MQTTClient::connect(const std::string& host, const unsigned int& port, const std::string& clientName)
{
    const std::string uri = "tcp://" + host + ":" + std::to_string(port);

    try {
        options = mqtt::create_options();

        mqttClient.reset(new mqtt::async_client(uri, clientName, options)); // , mqtt::create_options(3)
        mqttClient->set_callback(*this);

        connectOptions = mqtt::connect_options();
        connectOptions.set_keep_alive_interval(20);
        connectOptions.set_clean_session(true);
        mqttClient->connect(connectOptions);

    } catch (std::exception& e) {
        lastError = e.what();
        return false;
    }

    return true;
}
bool MQTTClient::subscribe(const std::string& topic, const MQTTCallback& cb)
{
    if (!mqttClient)
        return false;

    callbacks[topic] = cb;
    try {
        mqttClient->subscribe(topic, 1);
    } catch (std::exception& e) {
        lastError = e.what();
        return false;
    }

    return true;
}

void MQTTClient::unsubscribe(const std::string& topic)
{
    callbacks.erase(topic);
}

bool MQTTClient::publish(const std::string& k, const std::string& v)
{
    if (!mqttClient)
        return false;

    try {
        mqttClient->publish(k, v.data(), v.size());
    } catch (std::exception& e) {
        lastError = e.what();
        return false;
    }

    return true;
}

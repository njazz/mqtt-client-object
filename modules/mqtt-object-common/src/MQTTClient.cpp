#include "MQTTClient.hpp"

// ---
void MQTTClient::message_arrived(mqtt::const_message_ptr msg)
{
    onMessage(msg->get_topic(), msg->get_payload_str());
}

// ---

MQTTClient::~MQTTClient()
{
    disconnect();
}

bool MQTTClient::connect(const std::string& host, const unsigned int& port, const std::string& clientName, const std::string& userName, const std::string& password)
{
    const std::string uri = "tcp://" + host + ":" + std::to_string(port);

    try {
        options = mqtt::create_options();

        mqttClient.reset(new mqtt::async_client(uri, clientName, options)); // , mqtt::create_options(3)
        mqttClient->set_callback(*this);

        connectOptions = mqtt::connect_options();
        connectOptions.set_keep_alive_interval(20);
        connectOptions.set_clean_session(true);

        if (!userName.empty() && !password.empty()) {
            connectOptions.set_user_name(userName);
            connectOptions.set_password(password);
        }

        mqttClient->connect(connectOptions);

    } catch (std::exception& e) {
        lastError = e.what();
        return false;
    }

    return true;
}

bool MQTTClient::subscribe(const std::string& topic)
{
    if (!mqttClient)
        return false;

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
    if (!mqttClient)
        return;

    try {
        mqttClient->unsubscribe(topic);
    } catch (std::exception& e) {
        lastError = e.what();
        return;
    }
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

void MQTTClient::disconnect()
{
    if (!mqttClient)
        return;

    try {
        mqttClient->disconnect();
    }catch (std::exception& e) {
        lastError = e.what();
    }

    mqttClient.reset();
}

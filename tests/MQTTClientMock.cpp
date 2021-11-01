#include "MQTTClient.hpp"

class MQTTClientImplementation {};

MQTTClient::MQTTClient(){}

MQTTClient::~MQTTClient(){}

//void MQTTClient::message_arrived(mqtt::const_message_ptr /*msg*/) { }

// ---

bool MQTTClient::connect(const std::string& host, const unsigned int& port, const std::string& clientName, const std::string& username , const std::string& password )
{
    return true;
}
bool MQTTClient::subscribe(const std::string&)
{
    return true;
}
bool MQTTClient::publish(const std::string& k, const std::string& v)
{
    return true;
}

// ---
void MQTTClient::unsubscribe(const std::string&)
{
}
void MQTTClient::disconnect()
{
}

#include <ArduinoJson.h>
#include "SensorToolkitMqtt.h"

#include "config.h"
#include "DataReportingClient.h"

DataReportingClient::DataReportingClient(SensorToolkitMqtt& mqttClient){
    _mqttClient = &mqttClient;
}

boolean DataReportingClient::reportSensorData(Channel channel, unsigned long timestamp, double temperature) {
    // Assemble the message to be sent.
    StaticJsonDocument<200> json;
    
    json["timestamp"] = timestamp;
    json["temperature"] = temperature;

    serializeJsonPretty(json, Serial);
    Serial.println();
    serializeJson(json, _jsonOutput);
    
    // Publish the data on the topic that corresponds to the Channel provided.
    char *publishTopic;
    switch (channel) {
        default:
        case ONE:
            publishTopic = TOPIC_CHANNEL_1_DATA;
            break;
        case TWO:
            publishTopic = TOPIC_CHANNEL_2_DATA;
            break;
    }
    _mqttClient->publish(publishTopic, _jsonOutput);
}
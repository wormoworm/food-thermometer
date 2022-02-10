#include <ArduinoJson.h>
#include "SensorToolkitMqtt.h"

#include "config.h"
#include "DataReportingClient.h"

DataReportingClient::DataReportingClient(SensorToolkitMqtt& mqttClient, const char* reportingTopic){
    _mqttClient = &mqttClient;
    _reportingTopic = reportingTopic;
}

unsigned long DataReportingClient::getLastReportTimestampMs() {
    return _lastReportTimestampMs;
}

boolean DataReportingClient::shouldReportData(uint32_t minReportingIntervalMs) {
    return (millis() - _lastReportTimestampMs) > minReportingIntervalMs;
}

boolean DataReportingClient::reportData(unsigned long timestamp, double temperature) {
    // Assemble the message to be sent.
    StaticJsonDocument<200> json;
    
    json["timestamp"] = timestamp;
    json["temperature"] = temperature;

    serializeJsonPretty(json, Serial);
    Serial.println();
    serializeJson(json, _jsonOutput);
    
    // Publish the dataand update the publish time if successful.
    boolean status = _mqttClient->publish(_reportingTopic, _jsonOutput);
    if (status) _lastReportTimestampMs = millis();
    return status;
}
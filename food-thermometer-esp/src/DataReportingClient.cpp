#include <ArduinoJson.h>
#include "SensorToolkitMqtt.h"

#include "config.h"
#include "DataReportingClient.h"

DataReportingClient::DataReportingClient(SensorToolkitMqtt& mqttClient, const char* statusReportingTopic, const char* dataReportingTopic){
    _mqttClient = &mqttClient;
    _statusReportingTopic = statusReportingTopic;
    _dataReportingTopic = dataReportingTopic;
}

boolean DataReportingClient::reportStatus(unsigned long timestamp, boolean probeConnected) {
    StaticJsonDocument<200> json;

    json["timestamp"] = timestamp;
    json["probeConnected"] = probeConnected;

    serializeJson(json, _jsonOutput);
    
    // Publish the status.
    return _mqttClient->publish(_statusReportingTopic, _jsonOutput, true);
}

unsigned long DataReportingClient::getLastDataReportTimestampMs() {
    return _lastReportTimestampMs;
}

boolean DataReportingClient::shouldReportData(uint32_t minReportingIntervalMs) {
    return (millis() - _lastReportTimestampMs) > minReportingIntervalMs;
}

boolean DataReportingClient::reportData(unsigned long timestamp, double temperature) {
    StaticJsonDocument<200> json;
    
    json["timestamp"] = timestamp;
    json["temperature"] = temperature;

    serializeJson(json, _jsonOutput);
    
    // Publish the data and update the publish time if successful.
    boolean status = _mqttClient->publish(_dataReportingTopic, _jsonOutput, true);
    if (status) _lastReportTimestampMs = millis();
    return status;
}
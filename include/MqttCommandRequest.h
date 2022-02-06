#ifndef MqttCommandRequest_H
#define MqttCommandRequest_H

#include <ArduinoJson.h>

enum MqttCommandType {
    PUBLISH_CONFIG = 1
};

struct MqttCommandRequest {
    
    MqttCommandType type;
};

MqttCommandRequest mqttCommandRequestFromJson(JsonObject commandRequestJson) {
    return MqttCommandRequest { .type = commandRequestJson["type"] };
}

#endif
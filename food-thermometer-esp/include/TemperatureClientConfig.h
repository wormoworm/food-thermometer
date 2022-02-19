#ifndef TemperatureClientConfig_H
#define TemperatureClientConfig_H

#include <ArduinoJson.h>

struct TemperatureClientConfig {
    
    float c1;
    float c2;
    float c3;
};

TemperatureClientConfig temperatureClientConfigFromJson(JsonObject configJson);

#endif
#include <ArduinoJson.h>

#include "TemperatureClientConfig.h"

TemperatureClientConfig temperatureClientConfigFromJson(JsonObject configJson) {
    float c1 = configJson["c1"];
    float c2 = configJson["c2"];
    float c3 = configJson["c3"];
    return TemperatureClientConfig { .c1 = c1, .c2 = c2, .c3 = c3 };
}
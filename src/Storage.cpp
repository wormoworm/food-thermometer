#include <Preferences.h>

#include "Storage.h"
#include "TemperatureClientConfig.h"

Storage::Storage() {
    preferences.begin("storage");
}

boolean Storage::putTemperatureClientConfig(TemperatureClientConfig config, uint8_t channel) {
    if (channel < 0 || channel > 1) return false;
    const char *key = getTemperatureClientConfigKey(channel);
    size_t valueSize = sizeof(TemperatureClientConfig);
    uint8_t value[valueSize];
    memcpy(value, &config, valueSize);
    return preferences.putBytes(key, value, valueSize) != 0;
}

const TemperatureClientConfig Storage::getTemperatureClientConfig(uint8_t channel) {
    const char *key = getTemperatureClientConfigKey(channel);
    size_t valueSize = sizeof(TemperatureClientConfig);
    uint8_t bytes[valueSize];
    preferences.getBytes(key, bytes, valueSize);
    TemperatureClientConfig config;
    memcpy(&config, bytes, valueSize);
    Serial.print("Key: ");
    Serial.println(key);
    return config;
}

const char* Storage::getTemperatureClientConfigKey(uint8_t channel) {
    switch (channel) {
        case 1:
            return STORAGE_KEY_SH_COEFFICIENTS_CHANNEL_1;
        case 2:
            return STORAGE_KEY_SH_COEFFICIENTS_CHANNEL_2;
        default:
            return "error";
    }
}

#include <Preferences.h>

#include "Storage.h"
#include "TemperatureClientConfig.h"

Storage::Storage() {
    preferences.begin("storage");
}

boolean Storage::hasTemperatureClientConfig(Channel channel) {
    return preferences.isKey(getTemperatureClientConfigKey(channel));
}

boolean Storage::putTemperatureClientConfig(TemperatureClientConfig config, Channel channel) {
    if (channel < 1 || channel > 2) return false;
    const char *key = getTemperatureClientConfigKey(channel);
    size_t valueSize = sizeof(TemperatureClientConfig);
    uint8_t value[valueSize];
    memcpy(value, &config, valueSize);
    return preferences.putBytes(key, value, valueSize) != 0;
}

const TemperatureClientConfig Storage::getTemperatureClientConfig(Channel channel) {
    const char *key = getTemperatureClientConfigKey(channel);
    size_t valueSize = sizeof(TemperatureClientConfig);
    uint8_t bytes[valueSize];
    preferences.getBytes(key, bytes, valueSize);
    TemperatureClientConfig config;
    memcpy(&config, bytes, valueSize);
    return config;
}

const char* Storage::getTemperatureClientConfigKey(Channel channel) {
    switch (channel) {
        case CHANNEL_1:
            return STORAGE_KEY_SH_COEFFICIENTS_CHANNEL_1;
        case CHANNEL_2:
            return STORAGE_KEY_SH_COEFFICIENTS_CHANNEL_2;
        default:
            return "error";
    }
}

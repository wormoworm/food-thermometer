#ifndef Storage_H
#define Storage_H

#include <Preferences.h>

#include "TemperatureClientConfig.h"

#define STORAGE_KEY_SH_COEFFICIENTS_CHANNEL_1 "tempConfigCh1"
#define STORAGE_KEY_SH_COEFFICIENTS_CHANNEL_2 "tempConfigCh2"

class Storage {
    public:
        Storage();

        boolean putTemperatureClientConfig(TemperatureClientConfig config, uint8_t channel);

        const TemperatureClientConfig getTemperatureClientConfig(uint8_t channel);

    private:
        Preferences preferences;

        const char* getTemperatureClientConfigKey(uint8_t channel);
};

#endif
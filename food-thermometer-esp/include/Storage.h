#ifndef Storage_H
#define Storage_H

#include <Preferences.h>

#include "Channel.h"
#include "TemperatureClientConfig.h"

#define STORAGE_KEY_SH_COEFFICIENTS_CHANNEL_1 "tempConfigCh1"
#define STORAGE_KEY_SH_COEFFICIENTS_CHANNEL_2 "tempConfigCh2"

class Storage {
    public:
        Storage();

        boolean hasTemperatureClientConfig(Channel channel);

        boolean putTemperatureClientConfig(TemperatureClientConfig config, Channel channel);

        const TemperatureClientConfig getTemperatureClientConfig(Channel channel);

    private:
        Preferences preferences;

        const char* getTemperatureClientConfigKey(Channel channel);
};

#endif
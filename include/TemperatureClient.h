#ifndef TemperatureClient_H
#define TemperatureClient_H

#include <SmoothingBuffer.h>
#include "TemperatureClientConfig.h"

class TemperatureClient {

    public:
        TemperatureClient(uint8_t pinAdc, uint8_t pinPrescence, uint32_t dividerResistance, size_t smoothing);

        void setConfig(TemperatureClientConfig config);

        boolean isProbeConnected();

        float sampleTemperature();

        float getSmoothedTemperature();

    private:
        float getThermistorResistance();

        float calculateTemperature(float resistance);

        uint8_t _pinAdc;
        uint8_t _pinPrescence;
        uint32_t _dividerResistance;
        SmoothingBuffer _buffer;
        TemperatureClientConfig _config;
};

#endif
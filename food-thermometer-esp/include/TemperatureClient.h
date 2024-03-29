#ifndef TemperatureClient_H
#define TemperatureClient_H

#include "esp_adc_cal.h"
#include <SmoothingBuffer.h>
#include "TemperatureClientConfig.h"

#define TEMPERATURE_NOT_AVAILABLE 999

class TemperatureClient {

    public:
        TemperatureClient(adc1_channel_t adcChannel, uint8_t pinPrescence, uint32_t dividerResistance, size_t smoothing);

        void setConfig(TemperatureClientConfig config);

        boolean isProbeConnected();

        boolean sampleTemperature();

        float getSmoothedTemperature();

    private:
        float getThermistorResistance();

        float calculateTemperature(float resistance);

        adc1_channel_t _adcChannel;
        uint8_t _pinPrescence;
        uint32_t _dividerResistance;
        SmoothingBuffer _buffer;
        TemperatureClientConfig _config;

        esp_adc_cal_characteristics_t _adc_chars;
};

#endif
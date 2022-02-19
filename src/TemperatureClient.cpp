#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "TemperatureClient.h"
#include "TemperatureClientConfig.h"
#include "AdcCalibrationData.h"
#include "config.h"

TemperatureClient::TemperatureClient(adc1_channel_t adcChannel, uint8_t pinPrescence, uint32_t dividerResistance, size_t smoothing) : _buffer{smoothing} {
    _adcChannel = adcChannel;
    _pinPrescence = pinPrescence;
    _dividerResistance = dividerResistance;
    setConfig(TemperatureClientConfig { .c1 = TEMPERATURE_CLIENT_CONFIG_C1_DEFAULT, .c2 = TEMPERATURE_CLIENT_CONFIG_C2_DEFAULT, .c3 = TEMPERATURE_CLIENT_CONFIG_C3_DEFAULT });
    pinMode(_pinPrescence, INPUT);

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &_adc_chars);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(adcChannel, ADC_ATTEN_DB_11);
}

void TemperatureClient::setConfig(TemperatureClientConfig config) {
    _config = config;
}

boolean TemperatureClient::isProbeConnected() { 
    return digitalRead(_pinPrescence) == LOW;
}

boolean TemperatureClient::sampleTemperature() {
    if (isProbeConnected()) {
        float resistance = getThermistorResistance();
        float temperature = calculateTemperature(resistance);
        _buffer.storeValue(temperature);
        return true;
    }
    else {
        return false;
    }
}

float TemperatureClient::getSmoothedTemperature() {
    if (isProbeConnected()) {
        return _buffer.getAverage();
    }
    else{
        return TEMPERATURE_NOT_AVAILABLE;
    }
}

/**
 * @brief Corrects the ADC voltage using a polynomial equation.
 * This equation was derived by plotting voltage values (already corrected by the built-in ESP function esp_adc_cal_raw_to_voltage) against measurements taken using a DMM.
 * 
 * @param voltage       
 * @return uint32_t 
 */
uint32_t correctAdcVoltage(uint32_t voltage) {
    return (uint32_t) ((voltage * voltage * 0.00003) + (voltage * 0.8579) + 108.62);
}

float TemperatureClient::getThermistorResistance() {
    uint32_t rawReading =  adc1_get_raw(_adcChannel);
    uint32_t dividerVoltage = esp_adc_cal_raw_to_voltage(rawReading, &_adc_chars);
    uint32_t correctedDividerVoltage = correctAdcVoltage(dividerVoltage);
    uint32_t thermistorVoltage = ADC_VREF_MV - correctedDividerVoltage;
    float r1 = _dividerResistance / (float) correctedDividerVoltage * (float) thermistorVoltage;
    return r1;
}

float TemperatureClient::calculateTemperature(float resistance) {
    float logResistance = log(resistance);
    float temperature = (1.0 / (_config.c1 + _config.c2*logResistance + _config.c3*logResistance*logResistance*logResistance));
    return temperature - 273.15;
}
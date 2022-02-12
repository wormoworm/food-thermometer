#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "TemperatureClient.h"
#include "TemperatureClientConfig.h"
#include "AdcCalibrationData.h"

TemperatureClient::TemperatureClient(uint8_t pinAdc, uint8_t pinPrescence, uint32_t dividerResistance, size_t smoothing) : _buffer{smoothing} {
    _pinAdc = pinAdc;
    _pinPrescence = pinPrescence;
    _dividerResistance = dividerResistance;
    setConfig(TemperatureClientConfig { .c1 = 0.5821988606e-03, .c2 = 2.340232619e-04, .c3 = 0.7021977442e-07 });
    pinMode(_pinAdc, INPUT);
    pinMode(_pinPrescence, INPUT);

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &_adc_chars);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
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
        // Serial.print("Resistance: ");
        // Serial.println(resistance);
        float temperature = calculateTemperature(resistance);
        // Serial.print("Temperature: ");
        // Serial.println(temperature);
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

float correctResistance(float rawResistance) {
    return (rawResistance * 0.9376) + 4737.4;
}

uint32_t correctAdcVoltage(uint32_t voltage) {
    return (uint32_t) ((voltage * voltage * 0.00003) + (voltage * 0.8579) + 108.62);
}

// uint32_t correctAdcVoltage(uint32_t voltage) {
//     return (uint32_t) ((voltage * 0.9815) - 10.681);
// }

float TemperatureClient::getThermistorResistance() {
    uint32_t rawReading =  adc1_get_raw(ADC1_CHANNEL_3);
    float lutRawReadingValue = ADC_LUT[rawReading];
    uint16_t analogReadReading = analogRead(_pinAdc);
    float lutAnalogReadValue = ADC_LUT[analogReadReading];

    // Serial.print("adc1_get_raw: ");
    // Serial.println(rawReading);
    uint32_t espCalVoltage = esp_adc_cal_raw_to_voltage(rawReading, &_adc_chars);
    // Serial.print("espCalVoltage: ");
    // Serial.println(espCalVoltage);
    uint32_t correctedEspCalVoltage = correctAdcVoltage(espCalVoltage);
    // Serial.print("correctedEspCalVoltage: ");
    // Serial.println(correctedEspCalVoltage);

    // uint32_t espCalLutVoltage = esp_adc_cal_raw_to_voltage(lutRawReadingValue, &_adc_chars);
    // Serial.print("espCalLutVoltage: ");
    // Serial.println(espCalLutVoltage);

    // float correctedVoltage = voltage / 1.02;
    // Serial.print("correctedVoltage: ");
    // Serial.println(correctedVoltage);

    uint32_t thermistorVoltage = 3273 - correctedEspCalVoltage;
    // Serial.print("thermistorVoltage: ");
    Serial.println(thermistorVoltage);

    float r1 = _dividerResistance / (float) correctedEspCalVoltage * (float) thermistorVoltage;
    Serial.print("r1: ");
    Serial.println(r1);

    return r1;

    // float r = _dividerResistance * ((4013.0 / rawReading) - 1.0);
    // Serial.print("r: ");
    // Serial.println(r);

    // float resistance = _dividerResistance * ((4096.0 / rawReading) - 1.0);
    // float voltageResistance = _dividerResistance * ((4096.0 / voltage) - 1.0);
    // Serial.print("resistance: ");
    // Serial.println(resistance);
    // Serial.print("voltageResistance: ");
    // Serial.println(voltageResistance);

    // uint32_t lutVoltage = esp_adc_cal_raw_to_voltage(lutRawReadingValue, &_adc_chars);
    // Serial.print("lutVoltage: ");
    // Serial.println(lutVoltage);

    // Serial.print("analogReadReading: ");
    // Serial.println(analogReadReading);

    // Serial.print("lutRawReadingValue: ");
    // Serial.println(lutRawReadingValue);

    // Serial.print("lutAnalogReadValue: ");
    // Serial.println(lutAnalogReadValue);

    // float vOut = ADC_LUT[analogRead(_pinAdc)] *
    // float rawValue = ADC_LUT[analogRead(_pinAdc)];
    // return correctResistance(c);
}

// float TemperatureClient::getThermistorResistance() {
//     float rawValue = ADC_LUT[analogRead(_pinAdc)];
//     return correctResistance(_dividerResistance * ((4096.0 / rawValue) - 1.0));
// }

float TemperatureClient::calculateTemperature(float resistance) {
    float logResistance = log(resistance);
    float temperature = (1.0 / (_config.c1 + _config.c2*logResistance + _config.c3*logResistance*logResistance*logResistance));
    return temperature - 273.15;
}
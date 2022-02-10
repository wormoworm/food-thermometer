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

float TemperatureClient::getThermistorResistance() {
    uint16_t rawValue = analogRead(_pinAdc);
    return _dividerResistance * ((4096.0 / (float) rawValue) - 1.0);
}

float TemperatureClient::calculateTemperature(float resistance) {
    float logResistance = log(resistance);
    float temperature = (1.0 / (_config.c1 + _config.c2*logResistance + _config.c3*logResistance*logResistance*logResistance));
    return temperature - 273.15;
}
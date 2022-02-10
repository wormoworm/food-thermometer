#ifndef TemperatureDisplayClient_H
#define TemperatureDisplayClient_H

#include <DFRobot_LedDisplayModule.h>
#include <Arduino.h>

#define MAX_DISPLAY_DIGITS 4
#define FORMAT_VALUE_0DP "%.0f"
#define FORMAT_VALUE_1DP "%.1f"

struct TemperatureDisplayClientConfig {

    uint8_t precision;
};

class TemperatureDisplayClient {

    public:
        TemperatureDisplayClient(uint8_t address);

        boolean setConfig(TemperatureDisplayClientConfig config);

        void displayOff();

        void displayTemperature(double temperature);

        void displayNoTemperature();

        void displayCharacters(const char char1[3],const char char2[3],const char char3[3],const char char4[3]);

    private:

        uint8_t _address;
        TemperatureDisplayClientConfig _config;
        DFRobot_LedDisplayModule *_display;
        char _displayString[MAX_DISPLAY_DIGITS+2];

        uint8_t calculatePrecisionForValue(double temperature);

        double formatTemperatureValue(double temperature);

        void setDisplayAreaForDigits(uint8_t digits);
};

#endif
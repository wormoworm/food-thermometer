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

        void displayLoadingIndicator(uint8_t columns, boolean finalColumnActive);

        void displayTemperature(double temperature);

        void displayNoTemperature();

        void displayCharacters(size_t nChars, const char char1[] = "82",const char char2[] = "82",const char char3[] = "82",const char char4[] = "82");

        // void displayCharacters(const char chars[4][3], size_t nChars);

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
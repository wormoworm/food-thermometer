#include <ArduinoJson.h>
#include <DFRobot_LedDisplayModule.h>

#include "MathUtils.h"
#include "TemperatureDisplayClient.h"
#include "TemperatureClient.h"

TemperatureDisplayClient::TemperatureDisplayClient(uint8_t address) {
    _address = address;
    setConfig(TemperatureDisplayClientConfig { .precision = 1 });
}

boolean TemperatureDisplayClient::setConfig(TemperatureDisplayClientConfig config) {
    _config = config;
    _display = new DFRobot_LedDisplayModule(Wire, _address);
      /*Wait for the chip to be initialized completely, and then exit*/
    while(_display->begin4() != 0) {
        Serial.println("Failed to initialize the chip, please confirm the chip connection!");
        delay(1000);
    }
    return true;
}

void TemperatureDisplayClient::displayOff() {
    _display->displayOff();
}

void TemperatureDisplayClient::displayTemperature(double temperature) {
    // Ensure the display is switched on.
    _display->displayOn();

    if (temperature == TEMPERATURE_NOT_AVAILABLE) {
        displayNoTemperature();
    }
    else {
        // 1: Calculate how much precision we can display, based on the precision requested and the value.
        // 1.1: Calculate how many integral (LHS of decimal point) digits we need.
        uint8_t integralDigits;
        double absTemperature = abs(temperature);
        if (absTemperature >= 100) integralDigits = 3;
        else if (absTemperature >= 10) integralDigits = 2;
        else integralDigits = 1;
        // 1.2: For values < 0, add another digit to allow us to display the minus (-) sign.
        if (temperature < 0) integralDigits += 1;
        // 1.3: Now calculate if there is space to show the number of fractional digits requested.
        uint8_t fractionalDigits = _config.precision;
        if (integralDigits + fractionalDigits > MAX_DISPLAY_DIGITS) {
            fractionalDigits = MAX_DISPLAY_DIGITS - integralDigits;
        }
        uint8_t requiredDigits = integralDigits + fractionalDigits;

        // 2: Format the string to be displayed, using the number of fractional digits we just calculated.
        switch (fractionalDigits) {
            default:
            case 0:
                snprintf(_displayString, MAX_DISPLAY_DIGITS+2, FORMAT_VALUE_0DP, temperature);
                break;
            case 1:
                snprintf(_displayString, MAX_DISPLAY_DIGITS+2, FORMAT_VALUE_1DP, temperature);
                break;
        }
        
        int stringLength = strlen(_displayString);

        char displayCharacters[4][3];
        int stringPointer = 0;
        int displayCharacterPointer = 0;
        int characterPointer = 0;
        while(stringPointer < stringLength) {
            // Get the next character to process, and the one after it.
            char character = _displayString[stringPointer];
            // Store the character.
            displayCharacters[displayCharacterPointer][characterPointer++] = character;
            if (stringPointer < stringLength - 1) {
                // Check the next character, to see if it is a dot (.).
                char nextCharacter = _displayString[stringPointer + 1];
                if (nextCharacter == '.') {
                    displayCharacters[displayCharacterPointer][characterPointer++] = nextCharacter;
                    // Advance the pointer again, as we've now handled this next character.
                    stringPointer++;
                }
            }
            // Finally add the string delimiter and advance displayCharacterPointer.
            displayCharacters[displayCharacterPointer][characterPointer] = '\0';
            stringPointer++;
            displayCharacterPointer++;
            characterPointer = 0;          
        }

        // 3: Set the display area to fit the string we just formatted, and then display it.
        switch (requiredDigits) {
        case 1:
            _display->setDisplayArea4(4);
            _display->print4(displayCharacters[0]);
            break;
        case 2:
            _display->setDisplayArea4(3, 4);
            _display->print4(displayCharacters[0], displayCharacters[1]);
            break;
        case 3:
            _display->setDisplayArea4(2, 3, 4);
            _display->print4(displayCharacters[0], displayCharacters[1], displayCharacters[2]);
            break;
        default:
        case 4:
            _display->setDisplayArea4(1, 2, 3, 4);
            _display->print4(displayCharacters[0], displayCharacters[1], displayCharacters[2], displayCharacters[3]);
            break;
        }
    }
}

void TemperatureDisplayClient::displayNoTemperature() {
     // Set the display to show all four digits for the placeholder.
    _display->setDisplayArea4(1, 2, 3, 4);
    _display->print4("-", "-", "-", "-");
}

void TemperatureDisplayClient::displayCharacters(const char char1[3],const char char2[3],const char char3[3],const char char4[3]) {
    _display->setDisplayArea4(1, 2, 3, 4);
    _display->print4(char1, char2, char3, char4);
}

double TemperatureDisplayClient::formatTemperatureValue(double temperature) {
    return roundDouble(temperature, _config.precision);
}

uint8_t TemperatureDisplayClient::calculatePrecisionForValue(double temperature) {
    // First, calculate how many integral (LHS of decimal point) digits we need.
    uint8_t integralDigits;
    double absTemperature = abs(temperature);
    if (absTemperature >= 100) integralDigits = 3;
    else if (absTemperature >= 10) integralDigits = 2;
    else integralDigits = 1;

    // For values < 0, add another digit to allow us to display the minus (-) sign.
    if (temperature < 0) integralDigits += 1;

    // Now calculate if there is space to show the precision requested.
    uint8_t fractionalDigits = _config.precision;
    if (integralDigits + fractionalDigits > MAX_DISPLAY_DIGITS) {
        fractionalDigits = MAX_DISPLAY_DIGITS - integralDigits;
    }
    return fractionalDigits;
}

void TemperatureDisplayClient::setDisplayAreaForDigits(uint8_t digits) {
    
}

////////// TemperatureDisplayClientConfig //////////

TemperatureDisplayClientConfig temperatureDisplayClientConfigFromJson(JsonObject configJson) {
    uint8_t precision = configJson["precision"];
    return TemperatureDisplayClientConfig { .precision = precision };
}

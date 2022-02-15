#include "ChannelSelectClient.h"

ChannelSelectClient::ChannelSelectClient(uint8_t pinChannel1, uint8_t pinChannel2) {
    _pinChannel1 = pinChannel1;
    _pinChannel2 = pinChannel2;
    pinMode(_pinChannel1, INPUT);
    pinMode(_pinChannel2, INPUT);
}

Channel ChannelSelectClient::getSelectedChannel() {
    if (digitalRead(_pinChannel1) == HIGH) {
        return CHANNEL_1;
    }
    else if (digitalRead(_pinChannel2) == HIGH) {
        return CHANNEL_2;
    }
    else {
        return NONE;
    }
}
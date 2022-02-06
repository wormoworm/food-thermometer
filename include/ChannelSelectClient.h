#ifndef ChannelSelectClient_H
#define ChannelSelectClient_H

#include <Arduino.h>

#define CHANNEL_NOT_SELECTED -1
#define CHANNEL_1 -1
#define CHANNEL_NOT_SELECTED -1

enum Channel {
    NONE = 0,
    ONE = 1,
    TWO = 2
};

class ChannelSelectClient {

    public:
        ChannelSelectClient(uint8_t pinChannel1, uint8_t pinChannel2);

        Channel getSelectedChannel();

    private:

        uint8_t _pinChannel1;
        uint8_t _pinChannel2;
};

#endif
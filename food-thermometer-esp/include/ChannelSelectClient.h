#ifndef ChannelSelectClient_H
#define ChannelSelectClient_H

#include <Arduino.h>

#include "Channel.h"

class ChannelSelectClient {

    public:
        ChannelSelectClient(uint8_t pinChannel1, uint8_t pinChannel2);

        Channel getSelectedChannel();

    private:

        uint8_t _pinChannel1;
        uint8_t _pinChannel2;
};

#endif
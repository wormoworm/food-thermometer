#ifndef DataReportingClient_H
#define DataReportingClient_H

#include <Arduino.h>

#include "SensorToolkitMqtt.h"
#include "Channel.h"

class DataReportingClient {

    public:
        DataReportingClient(SensorToolkitMqtt& mqttClient);

        boolean reportSensorData(Channel channel, unsigned long timestamp, double temperature);

    private:

        SensorToolkitMqtt *_mqttClient;

        char _jsonOutput[200];   // TODO: Should we be smarter about the size?
};

#endif
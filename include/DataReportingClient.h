#ifndef DataReportingClient_H
#define DataReportingClient_H

#include <Arduino.h>

#include "SensorToolkitMqtt.h"
#include "Channel.h"

class DataReportingClient {

    public:
        DataReportingClient(SensorToolkitMqtt& mqttClient, const char* reportingTopic);

        unsigned long getLastReportTimestampMs();

        boolean shouldReportData(uint32_t minReportingIntervalMs);

        boolean reportData(unsigned long timestamp, double temperature);

    private:

        SensorToolkitMqtt *_mqttClient;

        char *_reportingTopic;

        char _jsonOutput[200];   // TODO: Should we be smarter about the size?

        unsigned long _lastReportTimestampMs = 0;
};

#endif
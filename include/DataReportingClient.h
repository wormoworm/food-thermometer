#ifndef DataReportingClient_H
#define DataReportingClient_H

#include <Arduino.h>

#include "SensorToolkitMqtt.h"
#include "Channel.h"

class DataReportingClient {

    public:
        DataReportingClient(SensorToolkitMqtt& mqttClient, const char* statusReportingTopic, const char* dataReportingTopic);

        boolean reportStatus(unsigned long timestamp, boolean probeConnected);

        unsigned long getLastDataReportTimestampMs();

        boolean shouldReportData(uint32_t minReportingIntervalMs);

        boolean reportData(unsigned long timestamp, double temperature);

    private:

        SensorToolkitMqtt *_mqttClient;

        const char *_statusReportingTopic, *_dataReportingTopic;

        char _jsonOutput[200];   // TODO: Should we be smarter about the size?

        unsigned long _lastReportTimestampMs = 0;
};

#endif
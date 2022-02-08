#include <Arduino.h>
#include <ArduinoJson.h>
#include <SmoothingBuffer.h>
#include <SensorToolkitWifi.h>
#include <SensorToolkitMqtt.h>
#include <SensorToolkitTime.h>
#include <time.h>

#include "TemperatureClient.h"
#include "config.h"
#include "Storage.h"
#include "MqttCommandRequest.h"
#include "ChannelSelectClient.h"
#include "TemperatureDisplayClient.h"
#include "DataReportingClient.h"
#include "MathUtils.h"

const char loadingCharacters[4][3] = {"1", "-", "-", "-"};

// Base config. TODO: Some of this needs to be settable via MQTT.
int temperatureSmoothing = 50;
int samplingIntervalMs = 20;
int temperatureReportingIntervalMs = 1000;
// SteinhartHartCoefficients shCoefficientsSalterProbe = { 1.579665719e-03, 1.093362615e-04, 3.288101479e-07 };

// Storage allows us to persist config data in flash.
Storage *storage;

// Handles checking of the channel select SPDT switch.
ChannelSelectClient *channelSelectClient;

// Handles displaying of temperature data, including formatting and placeholder values.
TemperatureDisplayClient *temperatureDisplayClient;

// Handles publication of data.
DataReportingClient *dataReportingClient;

WiFiClient espClient;
SensorToolkitWifi *wifiClient;
SensorToolkitMqtt mqttClient = SensorToolkitMqtt(espClient, CONFIG_MQTT_BROKER_ADDRESS, CONFIG_MQTT_BROKER_PORT, CONFIG_MQTT_CLIENT_ID);

TemperatureClient temperatureChannel1 = TemperatureClient(PIN_CHANNEL_1_VALUE, PIN_CHANNEL_1_PRESCENCE, DIVIDER_RESISTANCE_CHANNEL_1, temperatureSmoothing);
TemperatureClient temperatureChannel2 = TemperatureClient(PIN_CHANNEL_2_VALUE, PIN_CHANNEL_2_PRESCENCE, DIVIDER_RESISTANCE_CHANNEL_2, temperatureSmoothing);

unsigned long lastTemperatureReportTimestampMs = 0;

void flashLed() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
}

void handleCommandRequest(MqttCommandRequest commandRequest) {
    Serial.print("Handling command with type: ");
    Serial.println(commandRequest.type);

    switch(commandRequest.type) {
        case PUBLISH_CONFIG:
            Serial.println("Received publish config command");
            break;
        default:
            Serial.print("Received unknown command type: ");
            Serial.println(commandRequest.type);
            break;
    }
}

JsonObject decodeJsonObject(byte* payload, unsigned int length) {
    DynamicJsonDocument json(length * 2);
    deserializeJson(json, (const byte *) payload, length);
    return json.as<JsonObject>();
}

void wifiConnectionTickCallback(uint16_t tickNumber) {
    Serial.println(tickNumber);
    temperatureDisplayClient->displayCharacters(loadingCharacters[tickNumber % 4], loadingCharacters[(tickNumber + 1) % 4], loadingCharacters[(tickNumber + 2) % 4], loadingCharacters[(tickNumber + 3) % 4]);
}

/**
 * @brief Callback that is invoked whenever a message is received on a topic that the MQTT client is subscribed to.
 * 
 * @param topic     The topic the message was received on.
 * @param payload   The body of the message.
 * @param length    Length of the message in bytes
 */
void mqttSubscriptionCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    
    Serial.println();
    Serial.println("-----------------------");

    // What we do with the received message depends on the topic it arrived on.
    if (strcmp(topic, TOPIC_COMMAND_REQUEST) == 0) {
        handleCommandRequest(mqttCommandRequestFromJson(decodeJsonObject(payload, length)));
    }
}

void subscribeToMqttTopics() {
    mqttClient.subscribe(TOPIC_COMMAND_REQUEST);
    mqttClient.subscribe(TOPIC_CHANNEL_1_CONFIG);
    mqttClient.subscribe(TOPIC_CHANNEL_2_CONFIG);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);

    // Initialise the various long-lived clients we need.
    storage = new Storage();
    channelSelectClient = new ChannelSelectClient(PIN_CHANNEL_SELECT_1, PIN_CHANNEL_SELECT_2);
    temperatureDisplayClient = new TemperatureDisplayClient(ADDRESS_DISPLAY);
    dataReportingClient = new DataReportingClient(mqttClient);
    wifiClient = new SensorToolkitWifi();

    // Connect to Wifi.
    wifiClient->setConnectionTickCallback(wifiConnectionTickCallback);
    wifiClient->connectToWifi(WIFI_SSID, WIFI_PASSWORD, true);
    // Sync time from NTP server.
    syncNtp(0, 3600, "pool.ntp.org", true);
    // Connect to MQTT.
    mqttClient.setCallback(mqttSubscriptionCallback);
    mqttClient.connect(MQTT_USERNAME, MQTT_PASSWORD, CONFIG_MQTT_KEEP_ALIVE);
    subscribeToMqttTopics();
}

void loop() {
    unsigned long timeMs = millis();

    // 1: Delay by sampling interval.
    delay(samplingIntervalMs);

    // 2: Sample both channels.
    temperatureChannel1.sampleTemperature();
    temperatureChannel2.sampleTemperature();

    // 3: Display selected temperature on screen.
    switch (channelSelectClient->getSelectedChannel()) {
        case ONE:
            temperatureDisplayClient->displayTemperature(temperatureChannel1.getSmoothedTemperature());
            break;
        case TWO:
            temperatureDisplayClient->displayTemperature(temperatureChannel2.getSmoothedTemperature());
            break;
        default:
            temperatureDisplayClient->displayOff();
            break;
    }

    // 4: Publish data if publish interval exceeded.
    if ((timeMs - lastTemperatureReportTimestampMs) > temperatureReportingIntervalMs) {
        if (temperatureChannel1.isProbeConnected()) {
            dataReportingClient->reportSensorData(ONE, getEpochTime(), roundDouble(temperatureChannel1.getSmoothedTemperature(), 1));
        }
        if (temperatureChannel2.isProbeConnected()) {
            dataReportingClient->reportSensorData(TWO, getEpochTime(), roundDouble(temperatureChannel2.getSmoothedTemperature(), 1));
        }
        lastTemperatureReportTimestampMs = timeMs;
    }

    // 5: Allow the MQTT client to perform any operations it needs to.
    mqttClient.loop();
}
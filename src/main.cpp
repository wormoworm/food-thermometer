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

IPAddress CONFIG_WIFI_IP_ADDRESS, CONFIG_WIFI_GATEWAY, CONFIG_WIFI_SUBNET;
boolean _1 = CONFIG_WIFI_IP_ADDRESS.fromString("10.0.1.80");
boolean _2 = CONFIG_WIFI_GATEWAY.fromString("10.0.0.1");
boolean _3 = CONFIG_WIFI_SUBNET.fromString("255.255.252.0");

// Base config. TODO: Some of this needs to be settable via MQTT.

// SteinhartHartCoefficients shCoefficientsSalterProbe = { 1.579665719e-03, 1.093362615e-04, 3.288101479e-07 };

// Storage allows us to persist config data in flash.
Storage *storage;

// Handles checking of the channel select SPDT switch.
ChannelSelectClient *channelSelectClient;

// Handles displaying of temperature data, including formatting and placeholder values.
TemperatureDisplayClient *temperatureDisplayClient;

// Handles publication of data for each channel.
DataReportingClient *dataReportingClientChannel1, *dataReportingClientChannel2;

WiFiClient espClient;
SensorToolkitWifi *wifiClient;
SensorToolkitMqtt mqttClient = SensorToolkitMqtt(espClient, CONFIG_MQTT_BROKER_ADDRESS, CONFIG_MQTT_BROKER_PORT, CONFIG_MQTT_CLIENT_ID);

TemperatureClient temperatureChannel1 = TemperatureClient(ADC_CHANNEL_CHANNEL_1_VALUE, PIN_CHANNEL_1_PRESCENCE, DIVIDER_RESISTANCE_CHANNEL_1, TEMPERATURE_SAMPLE_SIZE);
TemperatureClient temperatureChannel2 = TemperatureClient(ADC_CHANNEL_CHANNEL_2_VALUE, PIN_CHANNEL_2_PRESCENCE, DIVIDER_RESISTANCE_CHANNEL_2, TEMPERATURE_SAMPLE_SIZE);

void flashLed() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
}

void initialiseConfig() {
    // Check if there is stored config for each channel, and load it if there is.
    if (storage->hasTemperatureClientConfig(CHANNEL_1)) {
        temperatureChannel1.setConfig(storage->getTemperatureClientConfig(CHANNEL_1));
    }
    if (storage->hasTemperatureClientConfig(CHANNEL_2)) {
        Serial.println("Has config for channel 2");
        temperatureChannel1.setConfig(storage->getTemperatureClientConfig(CHANNEL_2));
    }
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
    temperatureDisplayClient->displayLoadingIndicator(1, (tickNumber / 20) % 2 == 0);
}

boolean handleChannelConfig(Channel channel, JsonObject configJson) {
    TemperatureClientConfig config = temperatureClientConfigFromJson(configJson["data"].as<JsonObject>());
    Serial.print("Config c1: ");
    Serial.println(config.c1);
    if (storage->putTemperatureClientConfig(config, channel)) {
        Serial.print("Config stored for channel ");
        Serial.println(channel);
        switch (channel) {
            case CHANNEL_1:
                temperatureChannel1.setConfig(config);
                break;
            case CHANNEL_2:
                temperatureChannel2.setConfig(config);
                break;
            default:
            case NONE:
                break;
        }
        return true;
    }
    else {
        return false;
    }
}

boolean matchesTopic(const char *string, const char *topic) {
    return strcmp(string, topic) == 0;
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
    if (matchesTopic(topic, TOPIC_COMMAND_REQUEST)) {       // Message is a command request
        handleCommandRequest(mqttCommandRequestFromJson(decodeJsonObject(payload, length)));
    }
    else if (matchesTopic(topic, TOPIC_CHANNEL_1_CONFIG)) {
        handleChannelConfig(CHANNEL_1, decodeJsonObject(payload, length));
    }
    else if (matchesTopic(topic, TOPIC_CHANNEL_2_CONFIG)) {
        Serial.print("Handling channel 2 config: ");
        boolean result = handleChannelConfig(CHANNEL_2, decodeJsonObject(payload, length));
        Serial.println(result);
    }
}

void subscribeToMqttTopics() {
    mqttClient.subscribe(TOPIC_COMMAND_REQUEST);
    mqttClient.subscribe(TOPIC_CHANNEL_1_CONFIG);
    mqttClient.subscribe(TOPIC_CHANNEL_2_CONFIG);
}

void connectToWiFi() {
    temperatureDisplayClient->displayLoadingIndicator(1, false);
    if (wifiClient->connectToWifi(WIFI_SSID, WIFI_PASSWORD, true)) {
        // Sync time from NTP server.
        temperatureDisplayClient->displayLoadingIndicator(2, true);
        syncNtp(0, 3600, "pool.ntp.org", true);

        // Connect to MQTT.
        temperatureDisplayClient->displayLoadingIndicator(3, true);
        mqttClient.setCallback(mqttSubscriptionCallback);
        mqttClient.connect(MQTT_USERNAME, MQTT_PASSWORD, CONFIG_MQTT_KEEP_ALIVE);
        subscribeToMqttTopics();
        delay(500);
        temperatureDisplayClient->displayOff();
    }
    else {
        Serial.println("Failed to connect to WiFi");
    }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);

    // Initialise the various long-lived clients we need.
    storage = new Storage();
    channelSelectClient = new ChannelSelectClient(PIN_CHANNEL_SELECT_1, PIN_CHANNEL_SELECT_2);
    temperatureDisplayClient = new TemperatureDisplayClient(ADDRESS_DISPLAY);
    dataReportingClientChannel1 = new DataReportingClient(mqttClient, TOPIC_CHANNEL_1_DATA);
    dataReportingClientChannel2 = new DataReportingClient(mqttClient, TOPIC_CHANNEL_2_DATA);

    // Setup Wi-Fi.
    wifiClient = new SensorToolkitWifi();
    wifiClient->setConnectionTickCallback(wifiConnectionTickCallback);
    wifiClient->setIpConfig(CONFIG_WIFI_IP_ADDRESS, CONFIG_WIFI_GATEWAY, CONFIG_WIFI_SUBNET);

    initialiseConfig();
}

void loop() {
    if (!wifiClient->isConnected()) {
        connectToWiFi();
    } 
    // 1: Delay by sampling interval (minus the min ADC sampling interval - see below for why).
    delay(max(TEMPERATURE_SAMPLING_INTERVAL_MS - MIN_ADC_SAMPLING_SEPARATION_MS, 0));

    // 2: Sample both channels.
    temperatureChannel1.sampleTemperature();
    delay(MIN_ADC_SAMPLING_SEPARATION_MS);        // Allow a short period of time between sampling different ADC channels, some reports online suggest this can help with noise.
    temperatureChannel2.sampleTemperature();

    float tempChannel1 = temperatureChannel1.getSmoothedTemperature();
    float tempChannel2 = temperatureChannel2.getSmoothedTemperature();

    // 3: Display selected temperature on screen, if enough time has elapsed since the last update.
    if (temperatureDisplayClient->shouldUpdateScreen(SCREEN_UPDATE_INTERVAL_MS)) {
        switch (channelSelectClient->getSelectedChannel()) {
            case CHANNEL_1:
                temperatureDisplayClient->displayTemperature(tempChannel1);
                break;
            case CHANNEL_2:
                temperatureDisplayClient->displayTemperature(tempChannel2);
                break;
            default:
                temperatureDisplayClient->displayOff();
                break;
        }
    }

    // 4: Publish data to each channel, if enough time has elapsed since the previous report (per channel).
    if (temperatureChannel1.isProbeConnected() && dataReportingClientChannel1->shouldReportData(DATA_REPORTING_INTERVAL_MS)) {
        dataReportingClientChannel1->reportData(millis(), roundDouble(tempChannel1, 1));
    }
    if (temperatureChannel2.isProbeConnected() && dataReportingClientChannel2->shouldReportData(DATA_REPORTING_INTERVAL_MS)) {
        dataReportingClientChannel2->reportData(millis(), roundDouble(tempChannel2, 1));
    }

    // 5: Allow the MQTT client to perform any operations it needs to.
    mqttClient.loop();
}
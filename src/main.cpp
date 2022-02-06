#include <Arduino.h>
#include <ArduinoJson.h>
#include <DFRobot_LedDisplayModule.h>
#include <SmoothingBuffer.h>
#include <SensorToolkitWifi.h>
#include <SensorToolkitMqtt.h>
#include "TemperatureClient.h"
#include "config.h"
#include "Storage.h"
#include "MqttCommandRequest.h"

// Base config. TODO: Some of this needs to be settable via MQTT.
int temperatureSmoothing = 20;
int samplingIntervalMs = 10;
int reportingIntervalMs = 1000;
// SteinhartHartCoefficients shCoefficientsSalterProbe = { 1.579665719e-03, 1.093362615e-04, 3.288101479e-07 };

// Channel one config
int thermistorPinChannel1 = A1;

// Channel two config. TODO: This is just a clone of channel 1 for now.
int thermistorPinChannel2 = A1;

// Storage allows us to persist config data in flash.
Storage *storage;

WiFiClient espClient;
SensorToolkitMqtt mqttClient = SensorToolkitMqtt(espClient, CONFIG_MQTT_BROKER_ADDRESS, CONFIG_MQTT_BROKER_PORT, CONFIG_MQTT_CLIENT_ID);
DFRobot_LedDisplayModule ledDisplay(Wire, 0x48);

TemperatureClient temperatureChannel1 = TemperatureClient(PIN_CHANNEL_1_VALUE, PIN_CHANNEL_1_PRESCENCE, DIVIDER_RESISTANCE_CHANNEL_1, temperatureSmoothing);
TemperatureClient temperatureChannel2 = TemperatureClient(PIN_CHANNEL_2_VALUE, PIN_CHANNEL_2_PRESCENCE, DIVIDER_RESISTANCE_CHANNEL_2, temperatureSmoothing);

char jsonOutput[200];

void initialiseLedDisplay() {
    /*Wait for the chip to be initialized completely, and then exit*/
  while(ledDisplay.begin4() != 0) {
    Serial.println("Failed to initialize the chip, please confirm the chip connection!");
    delay(1000);
  }
  ledDisplay.setDisplayArea4(1,2,3,4);
}

void displayTemperature(double temperature) {
    ledDisplay.print4(temperature);
}

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

void initialiseMqtt() {
    mqttClient.setCallback(mqttSubscriptionCallback);
    mqttClient.connect(MQTT_USERNAME, MQTT_PASSWORD, CONFIG_MQTT_KEEP_ALIVE);
    subscribeToMqttTopics();
    mqttClient.subscribe("sensors/environment/foodthermometer/channel/one/config");
}

void publishData(double temperature) {
    StaticJsonDocument<200> json;
    
    json["temperature"] = temperature;

    Serial.println("-----");
    serializeJsonPretty(json, Serial);
    serializeJson(json, jsonOutput);
    
    mqttClient.publish("sensors/environment/foodthermometer/channel/one/data", jsonOutput);
}

double roundDouble(double value, int places) {
    int factor = pow(10, places);
    return ((int) ((value * factor) + 0.5)) / (double) factor;
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    storage = new Storage();
    
    initialiseLedDisplay();
    connectToWifi(WIFI_SSID, WIFI_PASSWORD, true);
    initialiseMqtt();
}

void loop() {
    flashLed();

    for(int i=0; i<temperatureSmoothing; i++) {
        temperatureChannel1.sampleTemperature();
        delay(samplingIntervalMs);
    }
    Serial.print("Ch1 probe connected: ");
    Serial.println(temperatureChannel1.isProbeConnected());

    double temperature = roundDouble(temperatureChannel1.getSmoothedTemperature(), 1);

    publishData(temperature);
    displayTemperature(123);

    delay(reportingIntervalMs);
    mqttClient.loop();
}
#ifndef config_H
#define config_H

// Pins. We define them using manual numbers (i.e. not using the #defines in pins_arduino.h), as these are not correct for our board for some reason.
#define D10 17  // Not defined in pins_arduino.h
#define D11 16  // Not defined in pins_arduino.h 
#define PIN_BATTERY_VOLTAGE 36
#define PIN_CHANNEL_1_PRESCENCE 16
#define PIN_CHANNEL_2_PRESCENCE 17
#define PIN_CHANNEL_SELECT_1 13
#define PIN_CHANNEL_SELECT_2 14
#define ADC_CHANNEL_CHANNEL_1_VALUE ADC1_CHANNEL_6
#define ADC_CHANNEL_CHANNEL_2_VALUE ADC1_CHANNEL_3

// I2C address of the 4-digit LED display.
#define ADDRESS_DISPLAY 0x48

// Divider resistor values, used to scale voltages into ADC range.
#define DIVIDER_RESISTANCE_CHANNEL_1 99400
#define DIVIDER_RESISTANCE_CHANNEL_2 98700

//
#define ADC_VREF_MV 3273

// Minimum time between consecutive ADC samples. Some reports online suggest this can help with noise.
#define MIN_ADC_SAMPLING_INTERVAL_MS 5

// For MQTT broker connection.
#define CONFIG_MQTT_BROKER_ADDRESS "10.0.1.2"
#define CONFIG_MQTT_BROKER_PORT 8884
#define CONFIG_MQTT_CLIENT_ID "foodthermometer1"
#define CONFIG_MQTT_KEEP_ALIVE 10

// MQTT topics.
#define TOPIC_BASE "sensors/environment/" CONFIG_MQTT_CLIENT_ID
#define TOPIC_COMMAND_REQUEST TOPIC_BASE "/command/request"
#define TOPIC_COMMAND_RESPONSE TOPIC_BASE "/command/response"
#define TOPIC_CHANNEL_1_BASE TOPIC_BASE "/channel/1"
#define TOPIC_CHANNEL_2_BASE TOPIC_BASE "/channel/2"
#define TOPIC_CHANNEL_1_DATA TOPIC_CHANNEL_1_BASE "/data"
#define TOPIC_CHANNEL_1_CONFIG TOPIC_CHANNEL_1_BASE "/config"
#define TOPIC_CHANNEL_2_DATA TOPIC_CHANNEL_2_BASE "/data"
#define TOPIC_CHANNEL_2_CONFIG TOPIC_CHANNEL_2_BASE "/config"

#endif
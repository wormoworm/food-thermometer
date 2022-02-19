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
#define DIVIDER_RESISTANCE_CHANNEL_1 103200
#define DIVIDER_RESISTANCE_CHANNEL_2 98700

// ADC VREF (board-specific)
#define ADC_VREF_MV 3273

#define TEMPERATURE_SAMPLE_SIZE 50
#define TEMPERATURE_SAMPLING_INTERVAL_MS 5
// Minimum time between consecutive ADC samples. Some reports online suggest this can help with noise.
#define MIN_ADC_SAMPLING_SEPARATION_MS 2

// For static IP configuration on local LAN.
#define CONFIG_WIFI_IP_ADDRESS_STRING "10.0.1.80"
#define CONFIG_WIFI_GATEWAY_STRING "10.0.0.1"
#define CONFIG_WIFI_SUBNET_STRING "255.255.252.0"

// For MQTT broker connection.
#define CONFIG_MQTT_BROKER_ADDRESS "10.0.1.2"
#define CONFIG_MQTT_BROKER_PORT 8884
#define CONFIG_MQTT_CLIENT_ID "foodthermometer1"
#define CONFIG_MQTT_KEEP_ALIVE 10

// Time between data publications to MQTT
#define DATA_REPORTING_INTERVAL_MS 1000

// Time between screen updates
#define SCREEN_UPDATE_INTERVAL_MS 100

// MQTT topics.
#define TOPIC_BASE "sensors/environment/" CONFIG_MQTT_CLIENT_ID
#define TOPIC_COMMAND_REQUEST TOPIC_BASE "/command/request"
#define TOPIC_COMMAND_RESPONSE TOPIC_BASE "/command/response"
#define TOPIC_CHANNEL_1_BASE TOPIC_BASE "/channel/1"
#define TOPIC_CHANNEL_2_BASE TOPIC_BASE "/channel/2"
#define TOPIC_CHANNEL_1_STATUS TOPIC_CHANNEL_1_BASE "/status"
#define TOPIC_CHANNEL_1_DATA TOPIC_CHANNEL_1_BASE "/data"
#define TOPIC_CHANNEL_1_CONFIG TOPIC_CHANNEL_1_BASE "/config"
#define TOPIC_CHANNEL_2_STATUS TOPIC_CHANNEL_2_BASE "/status"
#define TOPIC_CHANNEL_2_DATA TOPIC_CHANNEL_2_BASE "/data"
#define TOPIC_CHANNEL_2_CONFIG TOPIC_CHANNEL_2_BASE "/config"

// Default config for temperature clients
#define TEMPERATURE_CLIENT_CONFIG_C1_DEFAULT 0.5821988606e-03
#define TEMPERATURE_CLIENT_CONFIG_C2_DEFAULT 2.340232619e-04
#define TEMPERATURE_CLIENT_CONFIG_C3_DEFAULT 0.7021977442e-07

#endif
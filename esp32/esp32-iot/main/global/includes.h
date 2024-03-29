#ifndef INCLUDES_H
#define INCLUDES_H

// Done to ensure WebSocket support
#ifndef CONFIG_HTTPD_WS_SUPPORT
#define CONFIG_HTTPD_WS_SUPPORT
#endif

/* === Includes === */

#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <istream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_adc/adc_continuous.h>
#include <esp_event.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <sdkconfig.h>
#include <soc/soc_caps.h>
#include <sys/time.h>
#include <lwip/err.h>
#include <lwip/sys.h>


#include <inttypes.h>
#include <math.h>
#include <soc/dac_channel.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_check.h>
#include <driver/dac_continuous.h>

#include <assert.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include <driver/gptimer.h>
#include <driver/dac_oneshot.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/encodings.h"

#include "string_format.h"

/* === Enums for config === */

#define WIFI_STATION_MODE 0
#define WIFI_AP_MODE 1

#define TICKS_MS(T) (T * configTICK_RATE_HZ / 1000) // Minimum T=10
#define MIN(X, Y) (X <= Y ? X : Y)

/* === CONFIG === */

// Debug
// #define TEST
// #define DEBUG_L1
// #define DEBUG_L2
#define DEBUG_REPEATING_LOG_SPARSITY 100

// WiFi
#define WIFI_MODE WIFI_STATION_MODE

// HTTP Server
#define HTTP_DATA_STREAM_BUFFER_SIZE 512 // Bytes

// Oscilloscope related
#define ADC_BUFFER_CHUNKS 10

// ADC directly related
#define SAMPLE_FREQ (40 * 1000)
#define FRAME_SIZE 256 // min 140
#define BIT_WIDTH ADC_BITWIDTH_12
#define ADC_CHANNELS {ADC_CHANNEL_0};

// Select one below
// #define ATTENUATION_DB_0
// #define ATTENUATION_DB_2_5
// #define ATTENUATION_DB_6
#define ATTENUATION_DB_11

/* === Calculations === */

#define VALUES_PER_CHUNK (FRAME_SIZE / SOC_ADC_DIGI_RESULT_BYTES)
#define ADC_BUFFER_SIZE (VALUES_PER_CHUNK * ADC_BUFFER_CHUNKS)
// #define DATA_BUFFER_SIZE (VALUES_PER_CHUNK * DATA_BUFFER_CHUNKS)
#define SAMPLES_LIMIT (ADC_BUFFER_SIZE - (2 * VALUES_PER_CHUNK))

#ifdef ATTENUATION_DB_0
#define ATTENUATION ADC_ATTEN_DB_0
#define ATTENUATION_V_MIN 0.1
#define ATTENUATION_V_MAX 0.95
#endif

#ifdef ATTENUATION_DB_2_5
#define ATTENUATION ADC_ATTEN_DB_2_5
#define ATTENUATION_V_MIN 0.1
#define ATTENUATION_V_MAX 1.25
#endif

#ifdef ATTENUATION_DB_6
#define ATTENUATION ADC_ATTEN_DB_6
#define ATTENUATION_V_MIN 0.15
#define ATTENUATION_V_MAX 1.75
#endif

#ifdef ATTENUATION_DB_11
#define ATTENUATION ADC_ATTEN_DB_11
#define ATTENUATION_V_MIN 0.15
#define ATTENUATION_V_MAX 2.45
#endif

#define ATTENUATION_V_DELTA (ATTENUATION_V_MAX - ATTENUATION_V_MIN)

// Actual memory usage:
// ADC_BUFFER_CHUNKS * (FRAME_SIZE / SOC_ADC_DIGI_RESULT_BYTES * 2 bytes + 8 bytes) => ADC_BUFFER_CHUNKS * (FRAME_SIZE bytes + 8 bytes)
// DATA_BUFFER_CHUNKS * (FRAME_SIZE / SOC_ADC_DIGI_RESULT_BYTES * (2 bytes + 8 bytes)) => DATA_BUFFER_CHUNKS * (FRAME_SIZE * 5 bytes)

#endif
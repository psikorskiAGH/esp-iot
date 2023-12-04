#ifndef INCLUDES_H
#define INCLUDES_H

/* === Includes === */

#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <istream>
#include <string>
#include <unordered_map>
#include <memory>
#include <utility>

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

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/encodings.h"

/* === Enums for config === */

#define WIFI_STATION_MODE 0
#define WIFI_AP_MODE 1

#define TICKS_MS(T) (T * configTICK_RATE_HZ / 1000)  // Minimum T=10
#define MIN(X, Y) (X <= Y ? X : Y)

/* === CONFIG === */

// Debug
// #define TEST
#define DEBUG

#ifdef DEBUG
#define REPEATING_LOG_SPARSITY 100
#endif

// WiFi
#define WIFI_MODE WIFI_STATION_MODE

// HTTP Server
#define HTTP_DATA_STREAM_BUFFER_SIZE 1024

// Oscilloscope related
#define ADC_BUFFER_CHUNKS 17
#define DATA_BUFFER_CHUNKS 16

// ADC directly related
#define SAMPLE_FREQ (20 * 1000)
#define FRAME_SIZE 128
#define ATTENUATION ADC_ATTEN_DB_0
#define BIT_WIDTH ADC_BITWIDTH_12
static adc_channel_t channel[1] = {ADC_CHANNEL_0};


/* === Calculations === */

#define VALUES_PER_CHUNK (FRAME_SIZE / SOC_ADC_DIGI_RESULT_BYTES)
#define ADC_BUFFER_SIZE (VALUES_PER_CHUNK * ADC_BUFFER_CHUNKS)
#define DATA_BUFFER_SIZE (VALUES_PER_CHUNK * DATA_BUFFER_CHUNKS)
#define SAMPLES_LIMIT (ADC_BUFFER_SIZE - VALUES_PER_CHUNK)

// Actual memory usage:
// ADC_BUFFER_CHUNKS * (FRAME_SIZE / SOC_ADC_DIGI_RESULT_BYTES * 2 bytes + 8 bytes) => ADC_BUFFER_CHUNKS * (FRAME_SIZE bytes + 8 bytes)
// DATA_BUFFER_CHUNKS * (FRAME_SIZE / SOC_ADC_DIGI_RESULT_BYTES * (2 bytes + 8 bytes)) => DATA_BUFFER_CHUNKS * (FRAME_SIZE * 5 bytes)

#endif
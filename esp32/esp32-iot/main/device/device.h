#ifndef DEVICE_H
#define DEVICE_H

#include "../global/includes.h"
#include "../api/path.h"
#include "field.h"
#include "response.h"
#include "device_data.h"

#define OSCILLOSCOPE_FRAME_SIZE SOC_ADC_DIGI_DATA_BYTES_PER_CONV * 16
#define OSCILLOSCOPE_SAMPLE_FREQ SOC_ADC_SAMPLE_FREQ_THRES_HIGH / 20 // see SOC_ADC_SAMPLE_FREQ_THRES_HIGH

#define OSCILLOSCOPE_GATHERING_TASK_PRIORITY 6 // see configMAX_PRIORITIES

namespace device
{

    /* === Device elements === */

    struct device_info_t
    {
        std::string name;
        std::unordered_map<std::string, std::string> other = {};
    };

    class DeviceData; // Forward declaration
    class DeviceConfig; // Forward declaration

    class Device : public api::Path
    {
    private:
        DeviceData* device_data;
        DeviceConfig* device_config;
    protected:
        device_info_t device_info;

    public:
        Device(device_info_t device_info);
        ~Device();
        virtual bool get_response_data_fields(DeviceResponseWithFields *response) = 0;
        virtual bool get_config(DeviceResponseWithMap *response) = 0;
        virtual api::Response *set_config(rapidjson::Value const &data) = 0;

        api::Response *GET();
    };
}

#endif
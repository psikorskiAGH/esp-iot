#ifndef DEVICE_H
#define DEVICE_H

#include "../global/includes.h"
#include "../api/path.h"
#include "data_response.h"
#include "config_response.h"
#include "response.h"
#include "paths.h"
#include "list.h"

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

    class DeviceData;   // Forward declaration
    class DeviceConfig; // Forward declaration
    class DeviceList;   // Forward declaration

    class Device : public api::Path
    {
        friend class DeviceConfig;
        friend class DeviceList;

    private:
        DeviceData *device_data;
        DeviceConfig *device_config;

        std::unordered_map<std::string, BaseConfigField *> config_fields;

    protected:
        device_info_t device_info;
        /// @brief
        /// @param field (takes ownership)
        void add_config_field(BaseConfigField *field)
        {
            config_fields[field->name] = field;
        };

    public:
        Device(device_info_t device_info);
        /// @brief Initialization for overriden functions
        void init() { init_config(); }
        ~Device();
        virtual bool get_response_data_fields(DeviceDataResponse *response) = 0;

        virtual void init_config(){};
        virtual void on_config_update(){};

        api::Response *GET();
    };
}

#endif
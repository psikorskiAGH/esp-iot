#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "../global/includes.h"
#include "../api/response.h"
#include "adc.h"
#include "../device/device.h"
#include "../device/response.h"
#include "../device/data_response.h"
#include "../device/config_response.h"

namespace oscilloscope
{
    class Oscilloscope : public device::Device
    {
        static bool oscilloscope_running;
        struct config_fields_t {
            device::EnumConfigField* mode;
            device::IntConfigField* samples_before;
            device::IntConfigField* samples_after;
            device::IntConfigField* trigger_threshold;
            device::EnumConfigField* trigger_edge;
        } config_fields;

    public:
        Oscilloscope();
        bool get_response_data_fields(device::DeviceDataResponse *response);
        bool get_config(device::DeviceResponseWithMap *response);
        api::Response *set_config(rapidjson::Value const &data);

        virtual void init_config();
        virtual void on_config_update();
    };

}

#endif
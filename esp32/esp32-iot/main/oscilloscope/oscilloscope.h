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
            device::DoubleConfigField* sample_freq;
            device::IntConfigField* samples_before;
            device::IntConfigField* samples_after;
            device::DoubleConfigField* trigger_threshold;
            device::EnumConfigField* trigger_edge;
        } config_fields;

    public:
        Oscilloscope();
        bool get_response_data_fields(device::DeviceDataResponse *response);

        virtual void init_config();
        virtual void on_config_update();
    };

}

#endif
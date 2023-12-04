#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "../global/includes.h"
#include "../api/response.h"
#include "adc.h"
#include "../device/device.h"
#include "../device/response.h"
#include "../device/field.h"

namespace oscilloscope
{
    class Oscilloscope : public device::Device
    {
        static bool oscilloscope_running;

    public:
        Oscilloscope();
        bool get_response_data_fields(device::DeviceResponseWithFields *response);
        bool get_config(device::DeviceResponseWithMap *response);
        api::Response *set_config(rapidjson::Value const &data);
    };

}

#endif
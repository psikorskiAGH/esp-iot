#ifndef CLOCK_H
#define CLOCK_H

#include "../global/includes.h"
#include "../device/device.h"

namespace testclock
{
    class Clock : public device::Device {
    public:
        Clock();
        bool get_response_data_fields(device::DeviceDataResponse *response);
    };
} // namespace testclock

#endif
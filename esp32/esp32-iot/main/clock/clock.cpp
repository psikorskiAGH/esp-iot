#include "clock.h"

namespace testclock
{
    Clock::Clock() : device::Device({.name = "clock"}) {
        device_info.other["description"] = "This is an additional milisecond clock on ESP32 platform.";
        device_info.other["author"] = "Piotr Sikorski";
    }

    bool Clock::get_response_data_fields(device::DeviceDataResponse *response) {
        struct timeval time_struct = {};
        gettimeofday(&time_struct, NULL);
        int64_t ms = (int64_t)time_struct.tv_sec * 1000L + ((int64_t)time_struct.tv_usec / 1000);

        response->add_field(device::SingleDataField<const int64_t>("time", "ms", ms));
        return true;
    }
} // namespace testclock

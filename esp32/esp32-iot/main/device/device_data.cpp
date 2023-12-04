#include "device_data.h"

namespace device
{
    api::Response *DeviceData::GET()
    {
        DeviceResponseWithFields *response = new DeviceResponseWithFields();
        parent->get_response_data_fields(response);
        return response;
    }

    api::Response *DeviceConfig::GET()
    {
        DeviceResponseWithMap *response = new DeviceResponseWithMap();
        parent->get_config(response);
        return response;
    }

    api::Response *DeviceConfig::POST(rapidjson::Value const &data)
    {
        return parent->set_config(data);
    }
} // namespace device

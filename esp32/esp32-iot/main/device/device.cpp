#include "device.h"

namespace device
{
    Device::Device(device_info_t device_info) : api::Path(device_info.name), device_info(device_info){
        device_data = new DeviceData(this);
        device_config = new DeviceConfig(this);
        this->register_child_path(device_data);
        this->register_child_path(device_config);
    };
    Device::~Device(){
        delete device_data;
        delete device_config;
    };  

    api::Response *Device::GET()
    {
        DeviceResponseWithMap *response = new DeviceResponseWithMap();
        response->add_pair("name", device_info.name);
        for (auto &&kv : device_info.other)
        {
            response->add_pair(kv.first, kv.second);
        }
        return response;
    }
} // namespace device

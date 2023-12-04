#ifndef DEVICE_DATA_H
#define DEVICE_DATA_H

#include "../global/includes.h"
#include "device.h"

namespace device
{
    class Device; // Forward declaration

    class DeviceData : public api::Path
    {
        Device *parent;

    public:
        DeviceData(Device *parent) : api::Path(std::move("data")), parent(parent) {}
        ~DeviceData(){};

        api::Response *GET();
    };

    class DeviceConfig : public api::Path
    {
        Device *parent;

    public:
        DeviceConfig(Device *parent) : api::Path(std::move("config")), parent(parent) {}
        ~DeviceConfig(){};

        api::Response *GET();
        api::Response *POST(rapidjson::Value const &data);
    };
} // namespace device

#endif
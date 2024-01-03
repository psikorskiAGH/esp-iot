#ifndef DEVICE_LIST_H
#define DEVICE_LIST_H

#include "../api/path.h"
#include "device.h"

namespace device
{
    class Device; // Forward declaration

    class DeviceList : public api::Path
    {
        std::unordered_map<std::string, Device *> devices = {};

    public:
        DeviceList() : api::Path("devices") {}
        ~DeviceList();

        /// @brief
        /// @param dev (takes ownership)
        void add_device(Device *dev);
        virtual api::Response *GET();
    };
} // namespace device

#endif
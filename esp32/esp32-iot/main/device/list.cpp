#include "list.h"

namespace device
{
    DeviceList::~DeviceList()
    {
        for (auto &&dev : devices)
        {
            delete dev.second;
        }
    }
    void DeviceList::add_device(Device *dev)
    {
        this->devices[dev->device_info.name] = dev;
        this->register_child_path(dev);
        dev->init();
    }
    api::Response *DeviceList::GET()
    {
        rapidjson::Document *body = new rapidjson::Document(rapidjson::Type::kArrayType);
        RAPIDJSON_DEFAULT_ALLOCATOR *alloc = &body->GetAllocator();

        for (auto &&dev : devices)
        {
            body->PushBack(rapidjson::StringRef(dev.first.c_str(), dev.first.length()), *alloc);
        }
        
        return new api::ResponseOK(body);
    }
} // namespace device

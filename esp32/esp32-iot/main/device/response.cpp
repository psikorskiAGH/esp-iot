#include "response.h"

namespace device
{

    /* === DeviceResponseWithMap === */

    DeviceResponseWithMap::DeviceResponseWithMap() : api::Response("200 OK")
    {
        body_cache = new api::ResponseBody(rapidjson::Type::kObjectType);
        alloc = &body_cache->GetAllocator();
    };

    api::ResponseBody *DeviceResponseWithMap::create_body()
    {
        ESP_ERROR_CHECK(ESP_ERR_NOT_SUPPORTED);
        return nullptr;
    };

    void DeviceResponseWithMap::add_pair(std::string name, std::string value)
    {
        rapidjson::Value name_ref(name.c_str(), name.length(), *alloc); // Do copy
        rapidjson::Value val_ref(value.c_str(), value.length(), *alloc); // Do copy
        body_cache->AddMember(name_ref, val_ref, *alloc);
    }
    template <typename T>
    void DeviceResponseWithMap::add_pair(std::string name, T value)
    {
        rapidjson::Value name_ref(name.c_str(), name.length(), *alloc); // Do copy
        body_cache->AddMember(name_ref, value, *alloc);
    }
    template void DeviceResponseWithMap::add_pair(std::string name, int value);
    template void DeviceResponseWithMap::add_pair(std::string name, unsigned value);
    template void DeviceResponseWithMap::add_pair(std::string name, int64_t value);
    template void DeviceResponseWithMap::add_pair(std::string name, uint64_t value);

} // namespace device

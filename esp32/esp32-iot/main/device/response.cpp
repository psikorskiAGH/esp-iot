#include "response.h"

namespace device
{
    /* === DeviceResponseWithFields === */

    DeviceResponseWithFields::DeviceResponseWithFields() : api::Response("200 OK")
    {
        body_cache = new api::ResponseBody(rapidjson::Type::kArrayType);
        alloc = body_cache->GetAllocator();
    };

    api::ResponseBody *DeviceResponseWithFields::create_body()
    {
        ESP_ERROR_CHECK(ESP_ERR_NOT_SUPPORTED);
        return nullptr;
    };

    template <typename T>
    void DeviceResponseWithFields::add_field(SingleField<T> const &field)
    {
        rapidjson::Value field_body = rapidjson::Value(rapidjson::Type::kObjectType);
        field_body.AddMember(std::move("type"), "single", alloc);

        // Set name and unit
        rapidjson::Value name_v(rapidjson::Type::kStringType);
        name_v.SetString(field.name.c_str(), field.name.length(), alloc);
        field_body.AddMember(std::move("name"), name_v, alloc);

        rapidjson::Value unit_v;
        unit_v.SetString(field.unit.c_str(), field.unit.length(), alloc);
        field_body.AddMember(std::move("unit"), unit_v, alloc);

        rapidjson::Value value_v(field.value);
        field_body.AddMember(std::move("value"), value_v, alloc);

        body_cache->PushBack(field_body, alloc);
    }
    template void DeviceResponseWithFields::add_field(SingleField<const int> const &field);
    template void DeviceResponseWithFields::add_field(SingleField<const unsigned> const &field);
    template void DeviceResponseWithFields::add_field(SingleField<const int64_t> const &field);
    template void DeviceResponseWithFields::add_field(SingleField<const uint64_t> const &field);

    template <typename T>
    void DeviceResponseWithFields::add_field(ArrayField<T> const &field)
    {
        rapidjson::Value field_body = rapidjson::Value(rapidjson::Type::kObjectType);
        field_body.AddMember(std::move("type"), "array", alloc);

        // Set name and unit
        rapidjson::Value name_v(rapidjson::Type::kStringType);
        name_v.SetString(field.name.c_str(), field.name.length(), alloc);
        field_body.AddMember(std::move("name"), name_v, alloc);

        rapidjson::Value unit_v;
        unit_v.SetString(field.unit.c_str(), field.unit.length(), alloc);
        field_body.AddMember(std::move("unit"), unit_v, alloc);

        // Set values
        auto values = rapidjson::Value(rapidjson::Type::kArrayType);
        DataIterator<T> *iter = &field.values_iterator;
        size_t len = iter->length();
        values.Reserve(len, alloc);
        do
        {
            values.PushBack(iter->get(), alloc);
        } while (iter->next());
        field_body.AddMember(std::move("values"), std::move(values), alloc);

        body_cache->PushBack(field_body, alloc);
    }
    template void DeviceResponseWithFields::add_field(ArrayField<const int> const &field);
    template void DeviceResponseWithFields::add_field(ArrayField<const unsigned> const &field);
    template void DeviceResponseWithFields::add_field(ArrayField<const int64_t> const &field);
    template void DeviceResponseWithFields::add_field(ArrayField<const uint64_t> const &field);

    template <typename T>
    void DeviceResponseWithFields::add_field(TimeArrayField<T> const &field)
    {
        rapidjson::Value field_body = rapidjson::Value(rapidjson::Type::kObjectType);
        field_body.AddMember(std::move("type"), "timearray", alloc);

        // Set name and unit
        rapidjson::Value name_v(rapidjson::Type::kStringType);
        name_v.SetString(field.name.c_str(), field.name.length(), alloc);
        field_body.AddMember(std::move("name"), name_v, alloc);

        rapidjson::Value unit_v;
        unit_v.SetString(field.unit.c_str(), field.unit.length(), alloc);
        field_body.AddMember(std::move("unit"), unit_v, alloc);

        // Set values
        auto values = rapidjson::Value(rapidjson::Type::kArrayType);
        DataIterator<T> *iter_v = &field.values_iterator;
        size_t len = iter_v->length();
        values.Reserve(len, alloc);
        while (iter_v->next())
        {
            values.PushBack(iter_v->get(), alloc);
        };
        field_body.AddMember(std::move("values"), std::move(values), alloc);

        // Set timestamps
        auto timestamps = rapidjson::Value(rapidjson::Type::kArrayType);
        DataIterator<const int64_t> *iter = &field.timestamps_iterator;
        len = iter->length();
        timestamps.Reserve(len, alloc);
        while (iter->next())
        {
            timestamps.PushBack(iter->get(), alloc);
        };
        field_body.AddMember(std::move("timestamps"), std::move(timestamps), alloc);

        body_cache->PushBack(field_body, alloc);
    }
    template void DeviceResponseWithFields::add_field(TimeArrayField<const int> const &field);
    template void DeviceResponseWithFields::add_field(TimeArrayField<const unsigned> const &field);
    template void DeviceResponseWithFields::add_field(TimeArrayField<const int64_t> const &field);
    template void DeviceResponseWithFields::add_field(TimeArrayField<const uint64_t> const &field);

    /* === DeviceResponseWithMap === */

    DeviceResponseWithMap::DeviceResponseWithMap() : api::Response("200 OK")
    {
        body_cache = new api::ResponseBody(rapidjson::Type::kObjectType);
        alloc = body_cache->GetAllocator();
    };

    api::ResponseBody *DeviceResponseWithMap::create_body()
    {
        ESP_ERROR_CHECK(ESP_ERR_NOT_SUPPORTED);
        return nullptr;
    };

    void DeviceResponseWithMap::add_pair(std::string name, std::string value)
    {
        rapidjson::Value name_ref(name.c_str(), name.length(), alloc); // Do copy
        rapidjson::Value val_ref(value.c_str(), value.length(), alloc); // Do copy
        body_cache->AddMember(name_ref, val_ref, alloc);
    }
    template <typename T>
    void DeviceResponseWithMap::add_pair(std::string name, T value)
    {
        rapidjson::Value name_ref(name.c_str(), name.length(), alloc); // Do copy
        body_cache->AddMember(name_ref, value, alloc);
    }
    template void DeviceResponseWithMap::add_pair(std::string name, int value);
    template void DeviceResponseWithMap::add_pair(std::string name, unsigned value);
    template void DeviceResponseWithMap::add_pair(std::string name, int64_t value);
    template void DeviceResponseWithMap::add_pair(std::string name, uint64_t value);

} // namespace device

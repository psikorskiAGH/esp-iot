#include "data_response.h"

namespace device
{
    /* === DeviceDataResponse === */

    DeviceDataResponse::DeviceDataResponse() : api::Response("200 OK")
    {
        body_cache = new api::ResponseBody(rapidjson::Type::kArrayType);
        alloc = body_cache->GetAllocator();
    };

    api::ResponseBody *DeviceDataResponse::create_body()
    {
        ESP_ERROR_CHECK(ESP_ERR_NOT_SUPPORTED);
        return nullptr;
    };

    template <typename T>
    void DeviceDataResponse::add_field(SingleDataField<T> const &field)
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
    template void DeviceDataResponse::add_field(SingleDataField<const int> const &field);
    template void DeviceDataResponse::add_field(SingleDataField<const unsigned> const &field);
    template void DeviceDataResponse::add_field(SingleDataField<const int64_t> const &field);
    template void DeviceDataResponse::add_field(SingleDataField<const uint64_t> const &field);

    template <typename T>
    void DeviceDataResponse::add_field(ArrayDataField<T> const &field)
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

        rapidjson::Value min_v(field.v_min);
        field_body.AddMember(std::move("min"), min_v, alloc);
        
        rapidjson::Value max_v(field.v_max);
        field_body.AddMember(std::move("max"), max_v, alloc);

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
    template void DeviceDataResponse::add_field(ArrayDataField<const int> const &field);
    template void DeviceDataResponse::add_field(ArrayDataField<const unsigned> const &field);
    template void DeviceDataResponse::add_field(ArrayDataField<const int64_t> const &field);
    template void DeviceDataResponse::add_field(ArrayDataField<const uint64_t> const &field);

    template <typename T>
    void DeviceDataResponse::add_field(TimeArrayDataField<T> const &field)
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

        rapidjson::Value min_v(field.v_min);
        field_body.AddMember(std::move("min"), min_v, alloc);
        
        rapidjson::Value max_v(field.v_max);
        field_body.AddMember(std::move("max"), max_v, alloc);
        
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
    template void DeviceDataResponse::add_field(TimeArrayDataField<const int> const &field);
    template void DeviceDataResponse::add_field(TimeArrayDataField<const unsigned> const &field);
    template void DeviceDataResponse::add_field(TimeArrayDataField<const int64_t> const &field);
    template void DeviceDataResponse::add_field(TimeArrayDataField<const uint64_t> const &field);
    template void DeviceDataResponse::add_field(TimeArrayDataField<const double> const &field);
}
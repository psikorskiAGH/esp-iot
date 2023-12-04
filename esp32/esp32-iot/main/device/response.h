#ifndef DEVICE_RESPONSE_H
#define DEVICE_RESPONSE_H

#include "../global/includes.h"
#include "../global/types.h"
#include "../api/response.h"
#include "field.h"

namespace device
{
    class DeviceResponseWithFields : public api::Response
    {
    private:
        RAPIDJSON_DEFAULT_ALLOCATOR alloc;

    public:
        DeviceResponseWithFields();
        inline DeviceResponseWithFields operator=(DeviceResponseWithFields b) = delete;
        api::ResponseBody *create_body();

        template <typename T>
        void add_field(SingleField<T> const &field);

        template <typename T>
        void add_field(ArrayField<T> const &field);

        template <typename T>
        void add_field(TimeArrayField<T> const &field);
    };

    class DeviceResponseWithMap : public api::Response
    {
    private:
        RAPIDJSON_DEFAULT_ALLOCATOR alloc;

    public:
        DeviceResponseWithMap();
        inline DeviceResponseWithMap operator=(DeviceResponseWithMap b) = delete;
        api::ResponseBody *create_body();

        void add_pair(std::string name, std::string value);

        /// @brief
        /// @tparam T Only types available as rapidjson Value (there is overloaded function for string)
        /// @param name
        /// @param value
        template <typename T>
        void add_pair(std::string name, T value);
    };
} // namespace device

#endif
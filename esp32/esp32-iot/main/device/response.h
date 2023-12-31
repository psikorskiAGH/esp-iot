#ifndef DEVICE_RESPONSE_H
#define DEVICE_RESPONSE_H

#include "../global/includes.h"
#include "../global/types.h"
#include "../api/response.h"
#include "data_response.h"

namespace device
{
    // class DeviceDataResponse : public api::Response
    // {
    // private:
    //     RAPIDJSON_DEFAULT_ALLOCATOR alloc;

    // public:
    //     DeviceDataResponse();
    //     inline DeviceDataResponse operator=(DeviceDataResponse b) = delete;
    //     api::ResponseBody *create_body();

    //     template <typename T>
    //     void add_field(SingleDataField<T> const &field);

    //     template <typename T>
    //     void add_field(ArrayDataField<T> const &field);

    //     template <typename T>
    //     void add_field(TimeArrayDataField<T> const &field);
    // };

    // class DeviceConfigResponse : public api::Response
    // {
    // private:
    //     RAPIDJSON_DEFAULT_ALLOCATOR alloc;

    // public:
    //     DeviceConfigResponse();
    //     inline DeviceConfigResponse operator=(DeviceConfigResponse b) = delete;
    //     api::ResponseBody *create_body();

    //     template <typename T>
    //     void add_field(SingleDataField<T> const &field);

    //     template <typename T>
    //     void add_field(ArrayDataField<T> const &field);

    //     template <typename T>
    //     void add_field(TimeArrayDataField<T> const &field);
    // };

    class DeviceResponseWithMap : public api::Response
    {
    private:
        RAPIDJSON_DEFAULT_ALLOCATOR *alloc;

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
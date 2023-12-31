#ifndef DEVICE_DATA_RESPONSE_H
#define DEVICE_DATA_RESPONSE_H

#include "../global/includes.h"
#include "../global/types.h"
#include "../api/response.h"

namespace device
{
    /* === Fields === */

    template <typename T>
    class BaseDataField
    {
    public:
        std::string const name;
        std::string const unit;
        BaseDataField(std::string name, std::string unit) : name(name), unit(unit){};
    };

    template <typename T>
    class SingleDataField : public BaseDataField<T>
    {
    public:
        T const value;
        SingleDataField(std::string name, std::string unit, T value) : BaseDataField<T>(name, unit), value(value){};
    };

    template <typename T>
    class ArrayDataField : public BaseDataField<T>
    {
    public:
        T v_min;
        T v_max;
        DataIterator<const T> &values_iterator;
        ArrayDataField(
            std::string name,
            std::string unit,
            DataIterator<const T> &values_iterator,
            T v_min,
            T v_max)
            : BaseDataField<T>(name, unit),
              v_min(v_min),
              v_max(v_max),
              values_iterator(values_iterator){};
    };

    template <typename T>
    class TimeArrayDataField : public BaseDataField<T>
    {
    public:
        T v_min;
        T v_max;
        DataIterator<const T> &values_iterator;
        DataIterator<const int64_t> &timestamps_iterator;
        TimeArrayDataField(
            std::string name,
            std::string unit,
            DataIterator<const T> &values_iterator,
            DataIterator<const int64_t> &timestamps_iterator,
            T v_min,
            T v_max)
            : BaseDataField<T>(name, unit),
              v_min(v_min),
              v_max(v_max),
              values_iterator(values_iterator),
              timestamps_iterator(timestamps_iterator){};
    };

    /* === DeviceDataResponse === */

    class DeviceDataResponse : public api::Response
    {
    private:
        RAPIDJSON_DEFAULT_ALLOCATOR *alloc;

    public:
        DeviceDataResponse();
        inline DeviceDataResponse operator=(DeviceDataResponse b) = delete;
        api::ResponseBody *create_body();

        template <typename T>
        void add_field(SingleDataField<T> const &field);

        template <typename T>
        void add_field(ArrayDataField<T> const &field);

        template <typename T>
        void add_field(TimeArrayDataField<T> const &field);
    };
}

#endif
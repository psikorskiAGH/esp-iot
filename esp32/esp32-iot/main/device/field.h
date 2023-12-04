#ifndef DEVICE_FIELD_H
#define DEVICE_FIELD_H

#include "../global/includes.h"
#include "../global/types.h"

namespace device
{   
    template <typename T>
    class BaseField
    {
    public:
        std::string const name;
        std::string const unit;
        BaseField(std::string name, std::string unit) : name(name), unit(unit){};
    };

    template <typename T>
    class SingleField : public BaseField<T>
    {
    public:
        T const value;
        SingleField(std::string name, std::string unit, T value) : BaseField<T>(name, unit), value(value){};
    };

    template <typename T>
    class ArrayField : public BaseField<T>
    {
    public:
        DataIterator<const T> &values_iterator;
        ArrayField(std::string name, std::string unit, DataIterator<const T> &values_iterator)
            : BaseField<T>(name, unit), values_iterator(values_iterator){};
    };

    template <typename T>
    class TimeArrayField : public BaseField<T>
    {
    public:
        DataIterator<const T> &values_iterator;
        DataIterator<const int64_t> &timestamps_iterator;
        TimeArrayField(std::string name, std::string unit, DataIterator<const T> &values_iterator, DataIterator<const int64_t> &timestamps_iterator)
            : BaseField<T>(name, unit), values_iterator(values_iterator), timestamps_iterator(timestamps_iterator){};
    };
} // namespace device

#endif
#ifndef DEVICE_CONFIG_RESPONSE_H
#define DEVICE_CONFIG_RESPONSE_H

#include "../global/includes.h"
#include "../global/types.h"
#include "../api/response.h"
#include "../api/error.h"

namespace device
{

    /* === Base field === */

    class BaseConfigField
    {
    public:
        std::string const name;
        std::string const type;

        BaseConfigField(
            std::string name,
            std::string type)
            : name(name.c_str()), type(type.c_str())
        {
#ifdef DEBUG_L2
            ESP_LOGI("device.config", "Name: %s, Type: %s", name.c_str(), type.c_str());
#endif
        };
        virtual ~BaseConfigField(){};

        // virtual rapidjson::Value get_constrains(RAPIDJSON_DEFAULT_ALLOCATOR &alloc) = 0;
        virtual rapidjson::Value render(RAPIDJSON_DEFAULT_ALLOCATOR &alloc); // Has implementation but should be overriden incrementally
        virtual api::Error *stage_update(rapidjson::Value const &field) { return nullptr; };
        virtual void apply_update(){};
    };

    /* === Number field === */

    template <typename T>
    struct number_constrains_t
    {
        T min;
        T max;
        T step;
        bool validate(T value);
        rapidjson::Value render(RAPIDJSON_DEFAULT_ALLOCATOR &alloc);
    };

    template <typename T>
    class NumberConfigField : public BaseConfigField
    {
        std::string unit_staged;

    protected:
        T value_staged;
        number_constrains_t<T> constrains;

    public:
        std::string unit;
        T value;
        NumberConfigField(
            std::string name,
            std::string type,
            std::string unit,
            T value,
            number_constrains_t<T> constrains)
            : BaseConfigField(name, type), constrains(constrains), unit(unit), value(value){};

        virtual rapidjson::Value render(RAPIDJSON_DEFAULT_ALLOCATOR &alloc);
        virtual api::Error *stage_update(rapidjson::Value const &field);
        virtual void apply_update();
        virtual bool get_value_from_field(rapidjson::Value const &field, T *read_value) = 0;
    };

    class IntConfigField : public NumberConfigField<int64_t>
    {
    public:
        IntConfigField(
            std::string name,
            std::string unit,
            int64_t value,
            number_constrains_t<int64_t> constrains)
            : NumberConfigField(name, "integer", unit, value, constrains){};

        bool get_value_from_field(rapidjson::Value const &field, int64_t *read_value);
    };

    class DoubleConfigField : public NumberConfigField<double>
    {
    public:
        DoubleConfigField(
            std::string name,
            std::string unit,
            double value,
            number_constrains_t<double> constrains)
            : NumberConfigField(name, "float", unit, value, constrains){};

        bool get_value_from_field(rapidjson::Value const &field, double *read_value);
    };

    class EnumConfigField : public BaseConfigField
    {
    private:
        size_t index_staged;
        std::vector<std::string> const enum_values;

    public:
        size_t index;
        EnumConfigField(std::string name, std::vector<std::string> enum_values, size_t curr_index) : BaseConfigField(name, "enum"), enum_values(enum_values), index(curr_index){};
        rapidjson::Value render(RAPIDJSON_DEFAULT_ALLOCATOR &alloc);
        virtual api::Error *stage_update(rapidjson::Value const &field);
        virtual void apply_update();
    };

    /* === DeviceConfigResponse === */

    // class DeviceConfigResponse : public api::Response
    // {
    // private:
    //     RAPIDJSON_DEFAULT_ALLOCATOR alloc;

    // public:
    //     DeviceDataResponse();
    //     inline DeviceConfigResponse operator=(DeviceConfigResponse b) = delete;
    //     api::ResponseBody *create_body();

    //     template <typename T>
    //     void add_field(SingleDataField<T> const &field);

    //     template <typename T>
    //     void add_field(ArrayDataField<T> const &field);

    //     template <typename T>
    //     void add_field(TimeArrayDataField<T> const &field);
    // };
}

#endif
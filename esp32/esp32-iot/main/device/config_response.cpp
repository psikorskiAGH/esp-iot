#include "config_response.h"

namespace device
{
    rapidjson::Value BaseConfigField::render(RAPIDJSON_DEFAULT_ALLOCATOR &alloc)
    {
        rapidjson::Value v(rapidjson::kObjectType);
        v.AddMember("name", rapidjson::StringRef(name.c_str(), name.length()), alloc);
        v.AddMember("type", rapidjson::StringRef(type.c_str(), type.length()), alloc);
        return v;
    }

    /* === NumberConfigField === */

    template <typename T>
    rapidjson::Value NumberConfigField<T>::render(RAPIDJSON_DEFAULT_ALLOCATOR &alloc)
    {
        rapidjson::Value v = BaseConfigField::render(alloc);
        v.AddMember("unit", rapidjson::StringRef(unit.c_str(), unit.length()), alloc);
        v.AddMember("value", value, alloc);
        v.AddMember("constrains", constrains.render(alloc), alloc);
        return v;
    }

    template <typename T>
    api::Error *NumberConfigField<T>::stage_update(rapidjson::Value const &field)
    {
        auto err = BaseConfigField::stage_update(field);
        if (err != nullptr)
        {
            return err;
        }
        auto &&it = field.FindMember("unit");
        if (it == field.MemberEnd() || !it->value.IsString())
        {
            return new api::BadRequestError(string_format("Field %s: 'unit' key is missing or its value has incorrect type.", name));
        }
        unit_staged = std::string(it->value.GetString());

        auto &&it2 = field.FindMember("value");
        if (it2 == field.MemberEnd() || !get_value_from_field(it2->value, &value_staged))
        {
            return new api::BadRequestError(string_format("Field %s: 'value' key is missing or its value has incorrect type.", name));
        }
        if (!constrains.validate(value_staged))
        {
            return new api::BadRequestError(string_format("Field %s: value constrains are not satisfied.", name));
        }
        return nullptr;
    }

    template <typename T>
    void NumberConfigField<T>::apply_update()
    {
        BaseConfigField::apply_update();
        unit = unit_staged;
        value = value_staged;
    }

    template <typename T>
    bool number_constrains_t<T>::validate(T value)
    {
        return value <= max && value >= min && std::fmod((value - min), step) == 0;
    }

    template <typename T>
    rapidjson::Value number_constrains_t<T>::render(RAPIDJSON_DEFAULT_ALLOCATOR &alloc)
    {
        rapidjson::Value v(rapidjson::kObjectType);
        v.AddMember("min", min, alloc);
        v.AddMember("max", max, alloc);
        v.AddMember("step", step, alloc);
        return v;
    }

    /* === IntConfigField === */

    template class NumberConfigField<int64_t>;
    // template class number_constrains_t<int64_t>;

    bool IntConfigField::get_value_from_field(rapidjson::Value const &field, int64_t *read_value)
    {
        if (field.IsInt64())
        {
            (*read_value) = field.GetInt64();
            return true;
        }
        if (field.IsDouble())
        {
            double v = (field.GetDouble());
            if (v - (int64_t)v != 0)
            {
                return false;
            }
            (*read_value) = (int64_t)v;
            return true;
        }
        return false;
    }

    /* === DoubleConfigField === */

    template class NumberConfigField<double>;
    // template class number_constrains_t<double>;

    bool DoubleConfigField::get_value_from_field(rapidjson::Value const &field, double *read_value)
    {
        if (!field.IsDouble())
            return false;
        (*read_value) = field.GetDouble();
        return true;
    }

    /* === EnumConfigField === */

    rapidjson::Value EnumConfigField::render(RAPIDJSON_DEFAULT_ALLOCATOR &alloc)
    {
        rapidjson::Value v = BaseConfigField::render(alloc);
#ifdef DEBUG_L2
        printf("Enum index %u\r\n", index);
#endif
        const std::string *value = &enum_values[index];
#ifdef DEBUG_L2
        printf("Enum value %s\r\n", value->c_str());
#endif
        v.AddMember("value", rapidjson::StringRef(value->c_str(), value->length()), alloc);

        rapidjson::Value constrains(rapidjson::kArrayType);
        for (auto &&e : enum_values)
        {
#ifdef DEBUG_L2
            printf("%s\r\n", e.c_str());
#endif
            constrains.PushBack(rapidjson::StringRef(e.c_str(), e.length()), alloc);
        }
        v.AddMember("constrains", constrains, alloc);
        return v;
    }

    api::Error *EnumConfigField::stage_update(rapidjson::Value const &field)
    {
        auto err = BaseConfigField::stage_update(field);
        if (err != nullptr)
        {
            return err;
        }
        auto &&it = field.FindMember("value");
        if (it == field.MemberEnd() || !it->value.IsString())
        {
            return new api::BadRequestError(string_format("Field %s: 'value' key is missing or its value has incorrect type.", name));
        }
        std::string enum_name = it->value.GetString();
#ifdef DEBUG_L2
        printf("Enum %s\r\n", enum_name.c_str());
#endif

        size_t i;
        for (i = 0; i < enum_values.size(); i++)
        {
            if (enum_values[i] == enum_name)
            {
                break;
            }
        }

        if (i >= enum_values.size())
        {
            return new api::BadRequestError(string_format("Field %s: there is no enum value of ''.", name, enum_name));
        }
        index_staged = i;
#ifdef DEBUG_L2
        printf("index_staged %u\r\n", index_staged);
#endif
        return nullptr;
    }

    void EnumConfigField::apply_update()
    {
        BaseConfigField::apply_update();
#ifdef DEBUG_L2
        printf("index_staged %u, previous index %u\r\n", index_staged, index);
#endif
        index = index_staged;
    }
} // namespace device

#include "paths.h"

namespace device
{
    api::Response *DeviceData::GET()
    {
        DeviceDataResponse *response = new DeviceDataResponse();
        parent->get_response_data_fields(response);
        return response;
    }

    api::Response *DeviceConfig::GET()
    {
        // DeviceResponseWithMap *response = new DeviceResponseWithMap();
        // parent->get_config(response);
        // return response;

        rapidjson::Document* document = new rapidjson::Document(rapidjson::kArrayType);
        RAPIDJSON_DEFAULT_ALLOCATOR &alloc = document->GetAllocator();
        for (auto &&field : parent->config_fields)
        {
            document->PushBack(field.second->render(alloc), alloc);
        }
        return new api::ResponseOK(document);
    }

    api::Response *DeviceConfig::POST(rapidjson::Value const &data)
    {
        // return parent->set_config(data);
        if(!data.IsArray()) {
            return new api::BadRequestError(string_format("Config request must be Array, got RapidJSON type '%d'.", data.GetType()));
        }

        std::vector<device::BaseConfigField *> updated_config_fields = {};
        size_t index = 0;
        auto &&field = data.Begin();
        while (field != data.End())
        {
            if(!field->IsObject()) {
                return new api::BadRequestError(string_format("Config array does not have Object at index %d.", index));
            }
            auto &&it = field->FindMember("name");
            if (it == field->MemberEnd()) {
                return new api::BadRequestError(string_format("Object at index %d does not contain 'name' key.", index));
            }
            if (!it->value.IsString()) {
                return new api::BadRequestError(string_format("Object at index %d does contain invalid value for 'name' key.", index));
            }
            std::string name = std::string(it->value.GetString());
            auto &&config_it = parent->config_fields.find(name);
            if (config_it == parent->config_fields.end()) {
                return new api::BadRequestError(string_format("Config field with name '%s' does not exist.", name.c_str()));
            }
            auto err = config_it->second->stage_update(*field);
            if (err != nullptr) {
                return err;
            }
            updated_config_fields.push_back(config_it->second);
            ++field;
            ++index;
        }
        // Flush changes
        for (auto &&config_field : updated_config_fields)
        {
            config_field->apply_update();
        }
        parent->on_config_update();
        return new api::ResponseOK(new rapidjson::Document(rapidjson::kTrueType));
    }
} // namespace device

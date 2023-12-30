#include "oscilloscope.h"

namespace oscilloscope
{
    bool Oscilloscope::oscilloscope_running = false;

    Oscilloscope::Oscilloscope() : Device({.name = "oscilloscope"})
    {
        if (oscilloscope_running)
        {
            ESP_ERROR_CHECK(ESP_ERR_INVALID_STATE);
        }
        oscilloscope_running = true;
        start();
        device_info.other["model"] = "ESP32-WROOM-32D";
        device_info.other["description"] = "This is a built-in oscilloscope on ESP32 platform.";
        device_info.other["author"] = "Piotr Sikorski";

        config_fields = {
            .mode = new device::EnumConfigField(
                "mode",
                {"once", "value_trigger"},
                oscilloscope_data.get_mode()),
            .samples_before = new device::IntConfigField(
                "samples_before",
                "samples",
                oscilloscope_data.get_samples_before(),
                {.min = 0, .max = SAMPLES_LIMIT / 2, .step = 1}),
            .samples_after = new device::IntConfigField(
                "samples_after",
                "samples",
                oscilloscope_data.get_samples_after(),
                {.min = 0, .max = SAMPLES_LIMIT / 2, .step = 1}),
            .trigger_threshold = new device::IntConfigField(
                "trigger_threshold",
                "raw",
                oscilloscope_data.get_trigger_treshold(),
                {.min = 0, .max = 4095, .step = 1}),
            .trigger_edge = new device::EnumConfigField(
                "trigger_edge",
                {"rising", "falling"},
                oscilloscope_data.get_trigger_edge()),
        };
    };

    bool Oscilloscope::get_response_data_fields(device::DeviceDataResponse *response)
    {
        // No ownership claim over response

        // oscilloscope_data.set_samples_range(4, 4);

        oscilloscope_data.run();
        oscilloscope_data.wait_for_finish(TICKS_MS(5000));
        auto values_iter = oscilloscope_data.get_values_iterator();
        auto timestamps_iter = oscilloscope_data.get_timestamps_iterator();
        device::TimeArrayDataField<const double> f = device::TimeArrayDataField<const double>(
            "values", "V", values_iter, timestamps_iter, 0, 0.95);

        response->add_field(f);
        return true;
    }

    bool Oscilloscope::get_config(device::DeviceResponseWithMap *response)
    {
        std::string mode;
        switch (oscilloscope_data.get_mode())
        {
        case OscilloscopeModes::ONCE:
            mode = "once";
            break;
        case OscilloscopeModes::VALUE_TRIGGER:
            mode = "value_trigger";
            break;

        default:
            return false; // unknown error
            break;
        };
        response->add_pair("mode", mode);
        response->add_pair("samples_before", (unsigned int)oscilloscope_data.get_samples_before());
        response->add_pair("samples_after", (unsigned int)oscilloscope_data.get_samples_after());

        if (oscilloscope_data.get_mode() == OscilloscopeModes::VALUE_TRIGGER)
        {
            response->add_pair("trigger_threshold", (unsigned int)oscilloscope_data.get_trigger_treshold());
            std::string edge;
            switch (oscilloscope_data.get_trigger_edge())
            {
            case Edge::RISING:
                edge = "rising";
                break;
            case Edge::FALLING:
                edge = "falling";
                break;

            default:
                return false; // unknown error
                break;
            }
            response->add_pair("trigger_edge", edge);
        };
        return true;
    };

    api::Response *Oscilloscope::set_config(rapidjson::Value const &data)
    {
        if (!data.IsObject())
        {
            return new api::BadRequestError("Root JSON value must be object.");
        }
        size_t counted_fields = 0;
        rapidjson::Value::ConstMemberIterator el_it;
        rapidjson::Value const *v;
        rapidjson::Value::ConstMemberIterator end = data.MemberEnd();

        /* === Mode === */

        OscilloscopeModes mode;
        el_it = data.FindMember("mode");
        if (el_it != end)
        {
            ++counted_fields;
            v = &el_it->value;
            if (!v->IsString())
            {
                return new api::BadRequestError("'mode' must be string.");
            }
            const char *value = v->GetString();
            if (strcmp(value, "once"))
            {
                mode = OscilloscopeModes::ONCE;
            }
            else if (strcmp(value, "value_trigger"))
            {
                mode = OscilloscopeModes::VALUE_TRIGGER;
            }
            else
            {
                return new api::BadRequestError("Unknown mode, supported modes: 'once', 'value_trigger'.");
            }
        }
        else
        {
            mode = oscilloscope_data.get_mode();
        }
        if (!oscilloscope_data.set_mode(mode))
        {
            return new api::InternalServerError("Error while setting oscilloscope mode.");
        }

        /* === Samples === */

        size_t samples_before;
        el_it = data.FindMember("samples_before");
        if (el_it != end)
        {
            ++counted_fields;
            v = &el_it->value;
            if (!v->IsUint())
            {
                return new api::BadRequestError("'samples_before' must be unsigned integer.");
            }
            samples_before = v->GetUint();
        }
        else
        {
            samples_before = oscilloscope_data.get_samples_before();
        }
        size_t samples_after;
        el_it = data.FindMember("samples_after");
        if (el_it != end)
        {
            ++counted_fields;
            v = &el_it->value;
            if (!v->IsUint())
            {
                return new api::BadRequestError("'samples_after' must be unsigned integer.");
            }
            samples_after = v->GetUint();
        }
        else
        {
            samples_after = oscilloscope_data.get_samples_after();
        }
        if (!oscilloscope_data.set_samples_range(samples_before, samples_after))
        {
            return new api::BadRequestError("Error while setting oscilloscope samples range.");
        }

        if (mode == OscilloscopeModes::VALUE_TRIGGER)
        {
            /* === Trigger === */

            uint16_t trigger_threshold;
            el_it = data.FindMember("trigger_threshold");
            if (el_it != end)
            {
                ++counted_fields;
                v = &el_it->value;
                if (!v->IsUint())
                {
                    return new api::BadRequestError("'trigger_threshold' must be unsigned integer.");
                }
                trigger_threshold = v->GetUint();
            }
            else
            {
                trigger_threshold = oscilloscope_data.get_trigger_treshold();
            }
            Edge trigger_edge;
            el_it = data.FindMember("trigger_edge");
            if (el_it != end)
            {
                ++counted_fields;
                v = &el_it->value;
                if (!v->IsString())
                {
                    return new api::BadRequestError("'trigger_threshold' must be string.");
                }
                const char *value = v->GetString();
                if (strcmp(value, "rising"))
                {
                    trigger_edge = Edge::RISING;
                }
                else if (strcmp(value, "falling"))
                {
                    trigger_edge = Edge::FALLING;
                }
                else
                {
                    return new api::BadRequestError("Unknown trigger edge, supported modes: 'rising', 'falling'.");
                }
            }
            else
            {
                trigger_edge = oscilloscope_data.get_trigger_edge();
            }
            if (!oscilloscope_data.set_trigger(trigger_threshold, trigger_edge))
            {
                return new api::BadRequestError("Error while setting oscilloscope trigger.");
            }
        }

        if (counted_fields < data.MemberCount())
        {
            return new api::BadRequestError("Invalid fields exist in request.");
        }

        // api::NotFoundError *resp = new api::NotFoundError("Config setting not supported");
        api::ResponseBody *body = new api::ResponseBody(rapidjson::Type::kObjectType);
        body->AddMember("result", "success", body->GetAllocator());
        return new api::ResponseOK(body);
    };

    void Oscilloscope::init_config()
    {
        add_config_field(config_fields.mode);
        add_config_field(config_fields.samples_before);
        add_config_field(config_fields.samples_after);
        add_config_field(config_fields.trigger_threshold);
        add_config_field(config_fields.trigger_edge);
    }
    void Oscilloscope::on_config_update()
    {
        if (!oscilloscope_data.set_mode((OscilloscopeModes)config_fields.mode->index))
            ESP_LOGE("OSC", "Couldn't set mode - unexpected error.");
        if (!oscilloscope_data.set_samples_range(
                config_fields.samples_before->value,
                config_fields.samples_after->value))
            ESP_LOGE("OSC", "Couldn't set samples range - unexpected error.");
        if (!oscilloscope_data.set_trigger(
                config_fields.trigger_threshold->value,
                (Edge)config_fields.trigger_edge->index))
            ESP_LOGE("OSC", "Couldn't set trigger - unexpected error.");
    };
} // namespace oscilloscope

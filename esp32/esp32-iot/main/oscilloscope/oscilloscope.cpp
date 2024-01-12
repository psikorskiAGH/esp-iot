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
            .sample_freq = new device::DoubleConfigField(
                "sample_freq",
                "kHz",
                ((double)adc_cfg.sample_freq_hz) / 1000,
                {.min = 20, .max = 2000, .step = 0.1}),
            .samples_before = new device::IntConfigField(
                "samples_before",
                "samples",
                oscilloscope_data.get_samples_before(),
                {.min = 0, .max = SAMPLES_LIMIT / 2 - 1, .step = 1}),
            .samples_after = new device::IntConfigField(
                "samples_after",
                "samples",
                oscilloscope_data.get_samples_after(),
                {.min = 0, .max = SAMPLES_LIMIT / 2 - 1, .step = 1}),
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
            "voltage", "V", values_iter, timestamps_iter, ATTENUATION_V_MIN, ATTENUATION_V_MAX);

        response->add_field(f);
        return true;
    }

    void Oscilloscope::init_config()
    {
        add_config_field(config_fields.mode);
        add_config_field(config_fields.sample_freq);
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
        uint32_t new_sample_freq = (uint32_t)(config_fields.sample_freq->value * 1000);
        if (adc_cfg.sample_freq_hz != new_sample_freq)
        {
            adc_cfg.sample_freq_hz = new_sample_freq;
            adc_external_config_reload();
        }
    };
} // namespace oscilloscope

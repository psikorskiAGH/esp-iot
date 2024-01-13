#ifndef OSCILLOSCOPE_STRUCTS_H
#define OSCILLOSCOPE_STRUCTS_H

#include "../global/includes.h"

namespace oscilloscope
{
    enum OscilloscopeModes
    {
        ONCE,
        VALUE_TRIGGER
    };
    enum Edge
    {
        RISING,
        FALLING
    };

    struct adc_buffer_t
    {
        uint16_t values[ADC_BUFFER_SIZE] = {0};
        int64_t timestamps[ADC_BUFFER_CHUNKS] = {0};
        /// @brief Points to index at which new value should be written
        size_t index = 0;
    };

    struct run_result_t
    {
        int64_t start_time = 0;
        size_t first_index = 0;
        size_t length = 0;
    };

    struct settings_t
    {
        OscilloscopeModes mode = OscilloscopeModes::ONCE;
        // size_t samples_before = SAMPLES_LIMIT / 2 - 1;
        // size_t samples_after = SAMPLES_LIMIT / 2 - 1;
        // size_t samples_count = SAMPLES_LIMIT - 1;
        size_t samples_before = 300;
        size_t samples_after = 300;
        size_t samples_count = 601;
        uint16_t trigger_threshold = 2048; // in raw values
        Edge trigger_edge = Edge::RISING;
    };

    struct mode_trigger_data_t
    {
        bool first_cycle;
        bool should_finish;
        size_t finish_index;
    };
} // namespace oscilloscope

#endif
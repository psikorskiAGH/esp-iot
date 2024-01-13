#include "buffer.h"

namespace oscilloscope
{
    struct timeval last_conversion_time;

    /* === OscilloscopeBuffer === */

    bool OscilloscopeBuffer::is_running = false;
    SemaphoreHandle_t OscilloscopeBuffer::finished_run_semaphore = xSemaphoreCreateBinary();

    adc_buffer_t OscilloscopeBuffer::adc_buffer = {};
    run_result_t OscilloscopeBuffer::run_result = {};
    settings_t OscilloscopeBuffer::settings = {};
    mode_trigger_data_t OscilloscopeBuffer::mode_trigger_data = {};

    void OscilloscopeBuffer::start()
    {
#ifdef DEBUG_L1
        ESP_LOGI("adc_buf", "Starting Buffer: count %u, treshold %u", settings.samples_count, settings.trigger_threshold);
#endif
        struct timeval start_time_struct = {};
        adc_buffer.index = 0;
        mode_trigger_data.first_cycle = true;
        mode_trigger_data.should_finish = false;

        is_running = true;
        gettimeofday(&start_time_struct, NULL);
        run_result.start_time = (int64_t)last_conversion_time.tv_sec * 1000000L + (int64_t)last_conversion_time.tv_usec;
        adc_buffer.timestamps[ADC_BUFFER_CHUNKS - 1] = run_result.start_time;
#ifdef DEBUG_L2
        ESP_LOGI("adc_buf", "Started Buffer: is_running %u, start_time %lld", is_running, run_result.start_time);
#endif
    };

    void OscilloscopeBuffer::finish()
    {
#ifdef DEBUG_L1
        ESP_LOGI("adc_buf", "Finishing");
#endif
        is_running = false;
#ifndef DEBUG_L2
        xSemaphoreGive(finished_run_semaphore);
#else
        if (finished_run_semaphore == NULL)
        {
            ESP_LOGE("adc_buf", "Semaphore does not exist!");
        }
        else if (xSemaphoreGive(finished_run_semaphore) != pdTRUE)
        {
            ESP_LOGW("adc_buf", "Failed to give semaphore");
        };
#endif
    };

#ifdef DEBUG_L2
    static int counter_1 = 0;
    static int counter_2 = 0;
    static int counter_trigger_should_finish = 0;
#endif
    void OscilloscopeBuffer::analyze_new_data(size_t i_from, size_t i_to)
    {
#ifdef DEBUG_L2
        if (++counter_1 % DEBUG_REPEATING_LOG_SPARSITY == 0)
        {
            ESP_LOGI("adc_buf", "Analyzing from %u to %u", i_from, i_to);
        };
#endif
        switch (settings.mode)
        {
        case OscilloscopeModes::ONCE:
            // Assumes that data started from the 0 index
            if (i_to >= settings.samples_count)
            {
                run_result.first_index = 0;
                run_result.length = settings.samples_count;
#ifdef DEBUG_L2
                ESP_LOGI("adc_buf", "Once mode finished, first_index %u, length %u, start_time %lld",
                         run_result.first_index, run_result.length, run_result.start_time);
#endif
                finish();
            }
            break;
        case OscilloscopeModes::VALUE_TRIGGER:
            if (mode_trigger_data.should_finish)
            {
#ifdef DEBUG_L2
                ++counter_trigger_should_finish;
#endif
                if (i_to >= mode_trigger_data.finish_index && i_from <= mode_trigger_data.finish_index)
                {
#ifdef DEBUG_L2
                    ESP_LOGI("adc_buf", "Trigger mode finished, curr_index %u, first_index %u, length %u, start_time %lld",
                             adc_buffer.index, run_result.first_index, run_result.length, run_result.start_time);
                    ESP_LOGI("adc_buf", "finish_index: %u, i_from %u, i_to %u", mode_trigger_data.finish_index, i_from, i_to);
                    ESP_LOGI("adc_buf", "Sould finish exec count: %d", counter_trigger_should_finish);
                    counter_trigger_should_finish = 0;
#endif
                    finish();
                }
            }
            else if (mode_trigger_data.first_cycle)
            {
                if (adc_buffer.index >= settings.samples_before)
                {
                    mode_trigger_data.first_cycle = false;
                }
            }
            else
            {
                uint16_t prev_value = adc_buffer.values[(i_from + ADC_BUFFER_SIZE - 1) % ADC_BUFFER_SIZE];
                if (settings.trigger_edge == Edge::RISING)
                {
                    for (size_t i = i_from; i < i_to; i++)
                    {
                        uint16_t current_value = adc_buffer.values[i];
                        if (prev_value < settings.trigger_threshold && current_value >= settings.trigger_threshold)
                        {
                            // middle_index = i
                            run_result.first_index = (i - settings.samples_before + ADC_BUFFER_SIZE) % ADC_BUFFER_SIZE;
                            run_result.length = settings.samples_count;
                            mode_trigger_data.finish_index = (i + settings.samples_after + 1) % ADC_BUFFER_SIZE;
                            mode_trigger_data.should_finish = true;
                            if (i_to >= mode_trigger_data.finish_index && i_from <= mode_trigger_data.finish_index)  {
                                finish();
                            }
                            break;
                        }
                        prev_value = current_value;
                    }
                }
                else
                {
                    for (size_t i = i_from; i < i_to; i++)
                    {
                        uint16_t current_value = adc_buffer.values[i];
                        if (prev_value > settings.trigger_threshold && current_value <= settings.trigger_threshold)
                        {
                            // middle_index = i
                            run_result.first_index = (i - settings.samples_before + ADC_BUFFER_SIZE) % ADC_BUFFER_SIZE;
                            run_result.length = settings.samples_count;
                            mode_trigger_data.finish_index = (i + settings.samples_after) % ADC_BUFFER_SIZE;
                            mode_trigger_data.should_finish = true;
                            break;
                        }
                        prev_value = current_value;
                    }
                }
            }
            break;
        default:
            ESP_LOGE("adc_buf", "Unknown mode %u", settings.mode);
            finish();
            break;
        }
    };
    bool OscilloscopeBuffer::set_mode(OscilloscopeModes mode)
    {
        settings.mode = mode;
        if (mode == OscilloscopeModes::ONCE)
            settings.samples_after = 0;
        return true;
    }
    OscilloscopeBuffer::OscilloscopeBuffer()
    {
        // finished_run_semaphore = xSemaphoreCreateBinary();
    }
    void OscilloscopeBuffer::read_adc_data(uint8_t *data, size_t const &length)
    {
#ifdef DEBUG_L2
        if (++counter_2 % DEBUG_REPEATING_LOG_SPARSITY == 0)
        {
            ESP_LOGI("adc_buf", "Reading data of size %u/%u, is_running %u", length, FRAME_SIZE, is_running);
            // is_running = !is_running;
            // ESP_LOGI("adc_buf", "is_running %u", is_running);
        };
#endif
        if (!is_running)
            return;

        // Assert that always gets full buffer
        // ESP_ERROR_CHECK(length == FRAME_SIZE);
        // printf("%u", length);
        if (unlikely(length != FRAME_SIZE))
        {
            return;
        }

        // Get time here and interpolate time in-between previous call and now
        // That's the best one can make
        size_t index_from = adc_buffer.index;
        adc_buffer.timestamps[adc_buffer.index / VALUES_PER_CHUNK] = (int64_t)last_conversion_time.tv_sec * 1000000L + (int64_t)last_conversion_time.tv_usec;

        for (int i = 0; i < length; i += SOC_ADC_DIGI_RESULT_BYTES)
        {
            adc_digi_output_data_t *p = (adc_digi_output_data_t *)&data[i];
            uint16_t val = p->val;
            adc_buffer.values[adc_buffer.index] = val;
            adc_buffer.index++;
        }
        analyze_new_data(index_from, adc_buffer.index);
        adc_buffer.index %= ADC_BUFFER_SIZE;
    }
    bool OscilloscopeBuffer::set_samples_range(size_t samples_before, size_t samples_after)
    {
        // Guards
        if (samples_before + samples_after + 1 > SAMPLES_LIMIT)
            return false;
        settings.samples_before = samples_before;
        settings.samples_after = samples_after;
        settings.samples_count = samples_before + samples_after + 1;
        return true;
    }
    bool OscilloscopeBuffer::set_trigger(uint16_t treshold, Edge edge)
    {
        if (treshold > 4095)
            return false;
        settings.trigger_threshold = treshold;
        settings.trigger_edge = edge;
        return true;
    }

    bool OscilloscopeBuffer::run()
    {
#ifdef DEBUG_L1
        ESP_LOGI("adc_buf", "Running");
#endif
        if (is_running)
        {
#ifdef DEBUG_L1
            ESP_LOGW("adc_buf", "Already running");
#endif
            return false;
        }
        OscilloscopeBuffer::start();
        return true;
    }

    BaseType_t OscilloscopeBuffer::wait_for_finish(TickType_t timeout)
    {
#ifdef DEBUG_L2
        ESP_LOGI("adc_buf", "Waiting for finish (%lu ms), is_running %u", timeout / TICKS_MS(10) * 10, is_running);
#endif
        BaseType_t ret;
        if (is_running)
        {
            ret = xSemaphoreTake(finished_run_semaphore, timeout);
        }
        else
        {
            ret = pdTRUE;
        }
        // vTaskDelay(TICKS_MS(100));
        return ret;
    }
    BufferValuesIterator OscilloscopeBuffer::get_values_iterator()
    {
        return std::move(BufferValuesIterator(&adc_buffer, &run_result));
    };
    BufferTimestampsIterator OscilloscopeBuffer::get_timestamps_iterator()
    {
        return std::move(BufferTimestampsIterator(&adc_buffer, &run_result));
    };

} // namespace oscilloscope

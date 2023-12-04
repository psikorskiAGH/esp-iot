#ifndef OSCILLOSCOPE_BUFFER_H
#define OSCILLOSCOPE_BUFFER_H

#include "../global/includes.h"
#include "structs.h"
#include "iterators.h"

namespace oscilloscope
{
    extern struct timeval last_conversion_time;

    static class OscilloscopeBuffer
    {
        static bool is_running;
        static SemaphoreHandle_t finished_run_semaphore;

        static adc_buffer_t adc_buffer;
        static run_result_t run_result;
        static settings_t settings;
        static mode_trigger_data_t mode_trigger_data;

        void start();
        void finish();

        /// @brief
        /// @param i_from Index of first value
        /// @param i_to Index AFTER the last value, i_to > i_from
        void analyze_new_data(size_t i_from, size_t i_to);

    public:
        OscilloscopeBuffer();

        /// @brief
        /// @param data data from ADC (buffer)
        /// @param length length returned by ADC - asserts max length
        void read_adc_data(uint8_t *data, size_t const &length);

        /* === Settings setters and getters === */

        bool set_mode(OscilloscopeModes mode);
        bool set_samples_range(size_t samples_before, size_t samples_after = 0);
        bool set_trigger(uint16_t treshold, Edge edge);

        inline OscilloscopeModes get_mode() { return settings.mode; }
        inline size_t get_samples_before() { return settings.samples_before; }
        inline size_t get_samples_after() { return settings.samples_after; }
        inline uint16_t get_trigger_treshold() { return settings.trigger_threshold; }
        inline Edge get_trigger_edge() { return settings.trigger_edge; }

        /* === Run modes === */

        /// @brief
        /// @return did run start successfully
        bool run_once();
        bool run_trigger();

        /// @brief Waits for current run to finish. Works like a semaphore. Does not stop work if timeout happens.
        /// @param timeout timeout parameter to xSemaphoreTake()
        /// @return return value from xSemaphoreTake()
        BaseType_t wait_for_finish(TickType_t timeout);

        /* === Data iterators === */

        BufferValuesIterator get_values_iterator();
        BufferTimestampsIterator get_timestamps_iterator();

        /* === Statistics getters === */

    } oscilloscope_data;

} // namespace oscilloscope

#endif
#ifndef OSCILLOSCOPE_ITERATORS_H
#define OSCILLOSCOPE_ITERATORS_H

#include "../global/includes.h"
#include "../global/types.h"
#include "structs.h"

namespace oscilloscope
{
    class BufferValuesIterator : public DataIterator<const unsigned> // Unsigned because RapidJson
    {
    public:
        BufferValuesIterator(adc_buffer_t *adc_buffer_p, run_result_t *run_result);
        BufferValuesIterator(const BufferValuesIterator &&other) noexcept; // move constructor
        bool next();
        const unsigned &get() const;
        size_t length() const;

    private:
        adc_buffer_t *adc_buffer;
        size_t index;
        unsigned curr_value;
        size_t end_index;
        size_t len;
    };

    class BufferTimestampsIterator : public DataIterator<const int64_t>
    {
    public:
        BufferTimestampsIterator(adc_buffer_t *adc_buffer_p, run_result_t *run_result);
        BufferTimestampsIterator(const BufferTimestampsIterator &&other) noexcept; // move constructor
        
        bool next();
        const int64_t &get() const;
        size_t length() const;

    private:
        adc_buffer_t *adc_buffer;
        size_t len;
        int64_t curr_value;

        size_t buffer_index;
        size_t offset_index;
        size_t end_buffer_index;
        size_t end_offset_index;

        int64_t prev_buffer_timestamp;
        int64_t curr_buffer_timestamp;
        int64_t timestamp_delta;
    };
} // namespace oscilloscope

#endif
#include "iterators.h"

namespace oscilloscope
{
    /* === BufferValuesIterator === */

    BufferValuesIterator::BufferValuesIterator(adc_buffer_t *adc_buffer_p, run_result_t *run_result)
    {
        adc_buffer = adc_buffer_p;
        index = run_result->first_index - 1;
        curr_value = 0;
        end_index = (run_result->first_index + run_result->length) % ADC_BUFFER_SIZE;
        len = run_result->length;
#ifdef DEBUG
        ESP_LOGI("iter", "Created BufferValuesIterator, index %u, len %u, end %u", index, len, end_index);
#endif
    }
    BufferValuesIterator::BufferValuesIterator(const BufferValuesIterator &&other) noexcept // move constructor
    {
        adc_buffer = other.adc_buffer;
        len = other.len;
        index = other.index;
        curr_value = other.curr_value;
        end_index = other.end_index;
    }
    bool BufferValuesIterator::next()
    {
        if (index + 1 == end_index)
        {
#ifdef DEBUG
            ESP_LOGI("iter", "End of iterator");
#endif
            return false;
        }
        ++index;
        index %= ADC_BUFFER_SIZE;
        curr_value = adc_buffer->values[index];
        // printf("BufferValuesIterator::next(), index %u\r\n", index);
        return true;
    }
    const unsigned &BufferValuesIterator::get() const
    {
        return curr_value;
    }
    size_t BufferValuesIterator::length() const
    {
        return len;
    };

    /* === BufferTimestampsIterator === */

    BufferTimestampsIterator::BufferTimestampsIterator(adc_buffer_t *adc_buffer_p, run_result_t *run_result)
    {
        adc_buffer = adc_buffer_p;
        len = run_result->length;
        curr_value = 0;

        auto index_minus_1 = (run_result->first_index + ADC_BUFFER_SIZE - 1) % ADC_BUFFER_SIZE;
        buffer_index = index_minus_1 / VALUES_PER_CHUNK;
        offset_index = index_minus_1 % VALUES_PER_CHUNK;
        end_buffer_index = ((run_result->first_index + run_result->length) % ADC_BUFFER_SIZE) / VALUES_PER_CHUNK;
        end_offset_index = ((run_result->first_index + run_result->length) % ADC_BUFFER_SIZE) % VALUES_PER_CHUNK;

        prev_buffer_timestamp = offset_index == run_result->start_time;
        curr_buffer_timestamp = offset_index == VALUES_PER_CHUNK - 1 ? run_result->start_time : adc_buffer->timestamps[buffer_index];
        timestamp_delta = curr_buffer_timestamp - prev_buffer_timestamp;
#ifdef DEBUG
        ESP_LOGI("iter", "Created BufferTimestampsIterator, i_buffer %u, i_offset %u, len %u, end_buffer %u, end_offset %u",
                 buffer_index, offset_index, len, end_buffer_index, end_offset_index);
#endif
    }
    BufferTimestampsIterator::BufferTimestampsIterator(const BufferTimestampsIterator &&other) noexcept // move constructor
    {
        adc_buffer = other.adc_buffer;
        len = other.len;
        curr_value = other.curr_value;

        buffer_index = other.buffer_index;
        offset_index = other.offset_index;
        end_buffer_index = other.end_buffer_index;
        end_offset_index = other.end_offset_index;

        prev_buffer_timestamp = other.prev_buffer_timestamp;
        curr_buffer_timestamp = other.curr_buffer_timestamp;
        timestamp_delta = other.timestamp_delta;
    }
    bool BufferTimestampsIterator::next()
    {
        if (++offset_index >= VALUES_PER_CHUNK)
        {
            prev_buffer_timestamp = curr_buffer_timestamp;
            ++buffer_index;
            buffer_index %= ADC_BUFFER_CHUNKS;
            offset_index %= VALUES_PER_CHUNK;
            curr_buffer_timestamp = adc_buffer->timestamps[buffer_index];
            timestamp_delta = curr_buffer_timestamp - prev_buffer_timestamp;
#ifdef DEBUG
            ESP_LOGI("iter", "Next i_buffer %u, i_offset %u, t_prev %lld, t_curr %lld, t_delta %lld",
                     buffer_index, offset_index, prev_buffer_timestamp, curr_buffer_timestamp, timestamp_delta);
#endif
        }
        if (buffer_index == end_buffer_index && offset_index == end_offset_index)
        {
            --offset_index;
#ifdef DEBUG
            ESP_LOGI("iter", "End of iterator");
#endif
            return false;
        }
        curr_value = prev_buffer_timestamp + (offset_index + 1) * timestamp_delta / VALUES_PER_CHUNK;
        return true;
    }
    const int64_t &BufferTimestampsIterator::get() const
    {
        return curr_value;
    }
    size_t BufferTimestampsIterator::length() const
    {
        return len;
    }
} // namespace oscilloscope
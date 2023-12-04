#ifndef OSCILLOSCOPE_ADC_H
#define OSCILLOSCOPE_ADC_H

#include "../global/includes.h"
#include "buffer.h"

namespace oscilloscope
{
    extern TaskHandle_t s_task_handle;
    extern TaskHandle_t xHandle_adc_print;
    extern adc_continuous_handle_t adc_continuous_handle;

    extern bool conversion_done_interrupt(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data);
    extern void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle);

    extern void xTask_adc_run(void *params);

    extern void start();
}

#endif
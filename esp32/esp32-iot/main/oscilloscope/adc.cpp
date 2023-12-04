#include "adc.h"

namespace oscilloscope
{
    TaskHandle_t s_task_handle;
    TaskHandle_t xHandle_adc_print = NULL;
    adc_continuous_handle_t adc_continuous_handle = NULL;

    bool conversion_done_interrupt(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
    {
        gettimeofday(&last_conversion_time, NULL);
        BaseType_t mustYield = pdFALSE;
        // Notify that ADC continuous driver has done enough number of conversions
        vTaskNotifyGiveFromISR(s_task_handle, &mustYield);

        return (mustYield == pdTRUE);
    };

    void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle)
    {
        adc_continuous_handle_t handle = NULL;

        adc_continuous_handle_cfg_t adc_config = {
            .max_store_buf_size = 1024,
            .conv_frame_size = FRAME_SIZE,
        };
        ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

        adc_continuous_config_t dig_cfg = {
            .sample_freq_hz = SAMPLE_FREQ,
            .conv_mode = ADC_CONV_SINGLE_UNIT_1,
            .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        };

        adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
        dig_cfg.pattern_num = channel_num;
        for (int i = 0; i < channel_num; i++)
        {
            adc_pattern[i].atten = ATTENUATION;
            adc_pattern[i].channel = channel[i] & 0x7;
            adc_pattern[i].unit = ADC_UNIT_1;
            adc_pattern[i].bit_width = BIT_WIDTH;
        }
        dig_cfg.adc_pattern = adc_pattern;
        ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

        *out_handle = handle;
    }

    void xTask_adc_run(void *params)
    {
        esp_err_t ret;
        uint32_t ret_num = 0;
        uint8_t adc_result[FRAME_SIZE] = {0};
        memset(adc_result, 0xcc, FRAME_SIZE);

        s_task_handle = xTaskGetCurrentTaskHandle();

        continuous_adc_init(channel, sizeof(channel) / sizeof(adc_channel_t), &adc_continuous_handle);

        adc_continuous_evt_cbs_t cbs = {
            .on_conv_done = conversion_done_interrupt,
        };
        ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(adc_continuous_handle, &cbs, NULL));
        ESP_ERROR_CHECK(adc_continuous_start(adc_continuous_handle));

        while (1)
        {

            // Wait for enough conversions
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

            // char unit[] = "ADC_UNIT_1";

#ifdef DEBUG
            size_t i = 0;
#endif
            while (1)
            {
                ret = adc_continuous_read(adc_continuous_handle, adc_result, FRAME_SIZE, &ret_num, 0);
                if (ret == ESP_OK)
                {
                    oscilloscope_data.read_adc_data(adc_result, ret_num);
#ifdef DEBUG
                    if (++i % REPEATING_LOG_SPARSITY == 0)
                    {
                        ESP_LOGI("adc", "Data gathering loop %u, gathered %u",i, adc_result[0]);
                    }
                    vTaskDelay(1);
#endif
                }
                else if (ret == ESP_ERR_TIMEOUT)
                {
                    // We try to read `EXAMPLE_READ_LEN` until API returns timeout, which means there's no available data
                    break;
                }
            }
        }

        ESP_ERROR_CHECK(adc_continuous_stop(adc_continuous_handle));
        ESP_ERROR_CHECK(adc_continuous_deinit(adc_continuous_handle));
    }

    void start()
    {
#ifdef DEBUG
        ESP_LOGI("adc", "Starting ADC\r\n");
#endif
        xTaskCreate(xTask_adc_run, "adc_print", configMINIMAL_STACK_SIZE * 4, nullptr, 1, &xHandle_adc_print);
        configASSERT(xHandle_adc_print);
    }
}
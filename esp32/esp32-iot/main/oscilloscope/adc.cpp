#include "adc.h"

namespace oscilloscope
{
    TaskHandle_t s_task_handle;
    TaskHandle_t xHandle_adc_print = NULL;
    adc_continuous_handle_t adc_continuous_handle = NULL;
    adc_continuous_config_t adc_cfg = {
            .pattern_num = 0,
            .adc_pattern = NULL,
            .sample_freq_hz = SAMPLE_FREQ,
            .conv_mode = ADC_CONV_SINGLE_UNIT_1,
            .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        };
    
    struct {
        bool config_reload = false;
        SemaphoreHandle_t config_reloaded = NULL;
    } xSemaphore_adc;

    void adc_external_config_reload() {
        xSemaphore_adc.config_reload = true;
        xSemaphoreTake(xSemaphore_adc.config_reloaded, portMAX_DELAY);
    }

    bool conversion_done_interrupt(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
    {
        BaseType_t mustYield = pdFALSE;
        // Notify that ADC continuous driver has done enough number of conversions
        vTaskNotifyGiveFromISR(s_task_handle, &mustYield);

        return (mustYield == pdTRUE);
    };

    void continuous_adc_reconfig() {

    }

    void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle)
    {
        adc_continuous_handle_t handle = NULL;

        xSemaphore_adc.config_reloaded = xSemaphoreCreateBinary();

        adc_continuous_handle_cfg_t adc_config = {
            .max_store_buf_size = 1024,
            .conv_frame_size = FRAME_SIZE,
        };
        ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));


        adc_cfg.pattern_num = channel_num;

        adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX];
        for (int i = 0; i < channel_num; i++)
        {
            adc_pattern[i] = {.atten = ATTENUATION,
                              .channel = (uint8_t)(channel[i] & 0x7),
                              .unit = ADC_UNIT_1,
                              .bit_width = BIT_WIDTH,
            };
        }
        adc_cfg.adc_pattern = adc_pattern;
        ESP_ERROR_CHECK(adc_continuous_config(handle, &adc_cfg));

        *out_handle = handle;
    }

    static adc_channel_t channel[] = ADC_CHANNELS;
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
            .on_pool_ovf = NULL,
        };
        ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(adc_continuous_handle, &cbs, NULL));
        ESP_ERROR_CHECK(adc_continuous_start(adc_continuous_handle));

        while (1)
        {

            // Wait for enough conversions
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

            // char unit[] = "ADC_UNIT_1";

#ifdef DEBUG_L2
            size_t i = 0;
#endif
            while (1)
            {
                ret = adc_continuous_read(adc_continuous_handle, adc_result, FRAME_SIZE, &ret_num, 0);
                gettimeofday(&last_conversion_time, NULL);
                if (ret == ESP_OK)
                {
                    oscilloscope_data.read_adc_data(adc_result, ret_num);
#ifdef DEBUG_L2
                    if (++i % DEBUG_REPEATING_LOG_SPARSITY == 0)
                    {
                        ESP_LOGI("adc", "Data gathering loop %u, gathered %u", i, adc_result[0]);
                    }
                    vTaskDelay(1);
#endif
                }
                if (xSemaphore_adc.config_reload) {
                    ESP_ERROR_CHECK(adc_continuous_stop(adc_continuous_handle));
                    adc_cfg.adc_pattern[0].bit_width = BIT_WIDTH; // TODO: This is a bug
                    ESP_ERROR_CHECK(adc_continuous_config(adc_continuous_handle, &adc_cfg));
                    ESP_ERROR_CHECK(adc_continuous_start(adc_continuous_handle));
                    xSemaphore_adc.config_reload = false;
                    xSemaphoreGive(xSemaphore_adc.config_reloaded);
                }
                
                if (ret == ESP_ERR_TIMEOUT)
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
#ifdef DEBUG_L1
        ESP_LOGI("adc", "Starting ADC\r\n");
#endif
        xTaskCreate(xTask_adc_run, "xTask_adc_run", configMINIMAL_STACK_SIZE * 4, nullptr, 1, &xHandle_adc_print);
        configASSERT(xHandle_adc_print);
    }
}
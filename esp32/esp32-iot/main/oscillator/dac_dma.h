#ifndef OSCILLATOR_DAC_DMA_H
#define OSCILLATOR_DAC_DMA_H

#include "../global/includes.h"
#include "dac_example.h"

namespace oscillator
{
    /*
     * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: CC0-1.0
     */



#define EXAMPLE_WAVE_FREQ_HZ 2000                                          // Default wave frequency 2000 Hz, it can't be too low
#define EXAMPLE_CONVERT_FREQ_HZ (EXAMPLE_ARRAY_LEN * EXAMPLE_WAVE_FREQ_HZ) // The frequency that DAC convert every data in the wave array

    extern uint8_t sin_wav[EXAMPLE_ARRAY_LEN]; // Used to store sine wave values
    extern uint8_t tri_wav[EXAMPLE_ARRAY_LEN]; // Used to store triangle wave values
    extern uint8_t saw_wav[EXAMPLE_ARRAY_LEN]; // Used to store sawtooth wave values
    extern uint8_t squ_wav[EXAMPLE_ARRAY_LEN]; // Used to store square wave values

    static const char *TAG_DMA = "dac continuous(DMA)";
    static const char wav_name[4][15] = {"sine", "triangle", "sawtooth", "square"};

    static void dac_dma_write_task(void *args)
    {
        dac_continuous_handle_t handle = (dac_continuous_handle_t)args;
        uint8_t wav_sel = 0; // Start from sine wave

        size_t buf_len = EXAMPLE_ARRAY_LEN;

        while (1)
        {
            /* The wave in the buffer will be converted cyclically */
            switch (wav_sel)
            {
            case 0:
                ESP_ERROR_CHECK(dac_continuous_write_cyclically(handle, (uint8_t *)sin_wav, buf_len, NULL));
                break;
            case 1:
                ESP_ERROR_CHECK(dac_continuous_write_cyclically(handle, (uint8_t *)tri_wav, buf_len, NULL));
                break;
            case 2:
                ESP_ERROR_CHECK(dac_continuous_write_cyclically(handle, (uint8_t *)saw_wav, buf_len, NULL));
                break;
            case 3:
                ESP_ERROR_CHECK(dac_continuous_write_cyclically(handle, (uint8_t *)squ_wav, buf_len, NULL));
                break;
            default:
                break;
            }
            /* Switch wave every CONFIG_EXAMPLE_WAVE_PERIOD_SEC seconds */
            vTaskDelay(pdMS_TO_TICKS(CONFIG_EXAMPLE_WAVE_PERIOD_SEC * 1000));
            wav_sel++;
            wav_sel %= 4;
            ESP_LOGI(TAG_DMA, "%s wave start", wav_name[wav_sel]);
        }
    }

    void example_dac_continuous_by_dma(void)
    {
        dac_continuous_handle_t cont_handle;
        dac_continuous_config_t cont_cfg = {
            .chan_mask = DAC_CHANNEL_MASK_ALL,
            .desc_num = 8,
            .buf_size = 2048,
            .freq_hz = EXAMPLE_CONVERT_FREQ_HZ,
            .offset = 0,
            .clk_src = DAC_DIGI_CLK_SRC_DEFAULT, // If the frequency is out of range, try 'DAC_DIGI_CLK_SRC_APLL'
            /* Assume the data in buffer is 'A B C D E F'
             * DAC_CHANNEL_MODE_SIMUL:
             *      - channel 0: A B C D E F
             *      - channel 1: A B C D E F
             * DAC_CHANNEL_MODE_ALTER:
             *      - channel 0: A C E
             *      - channel 1: B D F
             */
            .chan_mode = DAC_CHANNEL_MODE_SIMUL,
        };
        /* Allocate continuous channel */
        ESP_ERROR_CHECK(dac_continuous_new_channels(&cont_cfg, &cont_handle));
        /* Enable the channels in the group */
        ESP_ERROR_CHECK(dac_continuous_enable(cont_handle));

        example_log_info(EXAMPLE_CONVERT_FREQ_HZ, EXAMPLE_WAVE_FREQ_HZ);

        /* Start to convert wave */
        xTaskCreate(dac_dma_write_task, "dac_dma_write_task", 4096, cont_handle, 5, NULL);
    }
} // namespace oscillator

#endif
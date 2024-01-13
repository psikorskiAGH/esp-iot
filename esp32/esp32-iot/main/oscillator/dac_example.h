#ifndef OSCILLATOR_DAC_EXAMPLE_H
#define OSCILLATOR_DAC_EXAMPLE_H

#include "../global/includes.h"

namespace oscillator
{
    /*
     * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
     *
     * SPDX-License-Identifier: CC0-1.0
     */
#define CONST_PERIOD_2_PI 6.2832 // 2 * PI

#define EXAMPLE_ARRAY_LEN 400     // Length of wave array
#define EXAMPLE_DAC_AMPLITUDE 240 // Amplitude of DAC voltage. If it's more than 256 will causes dac_output_voltage() output 0.
#define CONFIG_EXAMPLE_WAVE_PERIOD_SEC 10
#define CONFIG_EXAMPLE_DAC_CONTINUOUS_BY_DMA false

    /**
     * @brief Use DMA to convert continuously
     *
     */
    void example_dac_continuous_by_dma(void);

    /**
     * @brief Use timer to convert continuously
     *
     */
    void example_dac_continuous_by_timer(void);

    /**
     * @brief Print the example log information
     *
     * @param conv_freq     DAC conversion frequency
     * @param wave_freq     The frequency of the wave
     */
    void example_log_info(uint32_t conv_freq, uint32_t wave_freq);
} // namespace oscillator

#endif
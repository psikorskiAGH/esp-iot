/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */


#ifndef TEST

#include "global/includes.h"

#include "api/error.h"
#include "api/path.h"
#include "server/server.cpp"
#include "device/device.h"
#include "oscilloscope/oscilloscope.h"
#include "oscillator/dac.h"
#include "clock/clock.h"

#if WIFI_MODE == WIFI_STATION_MODE
#include "wifi/station.c"
#else
#include "wifi/ap.c"
#endif

extern "C"
{
    void app_main(void)
    {
        printf("Hello world!\n");

        oscillator::app_main();

#if WIFI_MODE == WIFI_STATION_MODE
        wifi_station_main();
#else
        wifi_ap_main();
#endif
        server::start();

        oscilloscope::Oscilloscope* osc = new oscilloscope::Oscilloscope();
        server::devices.add_device(osc);

        // testclock::Clock* clk = new testclock::Clock();
        // server::devices.add_device(clk);

        while (true)
        {
            // printf("Restarting in %d seconds...\n", i);
            vTaskDelay(portMAX_DELAY);
        }
        printf("Restarting now.\n");
        fflush(stdout);
        esp_restart();
    }
}

#endif // ifndef TEST

idf_component_register(
    SRCS "main.cpp" "api/error.cpp" "api/path.cpp" "api/response.cpp" "device/config_response.cpp" "device/data_response.cpp" "device/device.cpp" "device/paths.cpp" "device/response.cpp" "oscilloscope/adc.cpp" "oscilloscope/buffer.cpp" "oscilloscope/iterators.cpp" "oscilloscope/oscilloscope.cpp" "server/server.cpp"
    INCLUDE_DIRS ""
    REQUIRES esp_http_server
    REQUIRES esp_wifi
    REQUIRES nvs_flash
    REQUIRES esp_adc
)

#ifndef STRING_FORMAT_H
#define STRING_FORMAT_H

/*
Source: https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
*/

#include <memory>
#include <string>
#include <stdexcept>
#include <esp_err.h>

template <typename... Args>
std::string string_format(const std::string &format, Args... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if (size_s <= 0)
    {
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}



#endif
#ifndef TYPES_H
#define TYPES_H

#include <freertos/FreeRTOS.h>

template <typename T>
class DataIterator
{
public:
    DataIterator(){};
    ~DataIterator(){};

    DataIterator(const DataIterator &other) // copy constructor
        = delete;

    DataIterator(DataIterator &&other) noexcept // move constructor
        = delete;

    DataIterator &operator=(const DataIterator &other) // copy assignment
        = delete;

    DataIterator &operator=(DataIterator &&other) noexcept // move assignment
        = delete;

    virtual bool next() = 0;
    virtual T &get() const = 0;
    virtual size_t length() const = 0;
    T &operator*() const { return get(); }
};

#endif
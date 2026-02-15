#pragma once

#include <gpiod.h>
#include <string>

namespace hal {

class GpioIn {
public:
    GpioIn(const std::string& chipname,
            unsigned int line_offset,
            const std::string& consumer);

    ~GpioIn();

    GpioIn(const GpioIn&) = delete;
    GpioIn& operator=(const GpioIn&) = delete;

    GpioIn(GpioIn&& other) noexcept;
    GpioIn& operator=(GpioIn&& other) noexcept;

    bool read_digital();


private:
    gpiod_chip* chip_ = nullptr;
    gpiod_line_request* request_ = nullptr;
    unsigned int line_offset_;
};


} // namespace hal

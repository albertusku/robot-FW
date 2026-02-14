#pragma once

#include <gpiod.h>
#include <string>

namespace hal {

class GpioOut {
public:
    GpioOut(const std::string& chipname,
            unsigned int line_offset,
            const std::string& consumer);

    ~GpioOut();

    GpioOut(const GpioOut&) = delete;
    GpioOut& operator=(const GpioOut&) = delete;

    GpioOut(GpioOut&& other) noexcept;
    GpioOut& operator=(GpioOut&& other) noexcept;

    void set(bool value);

private:
    gpiod_chip* chip_ = nullptr;
    gpiod_line_request* request_ = nullptr;
    unsigned int line_offset_;
};

} // namespace hal

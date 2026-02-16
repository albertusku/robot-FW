#pragma once

#include <gpiod.h>
#include <vector>
#include <string>
#include <cstdint>

namespace hal {

struct GpioEvent {
    unsigned int line_offset;
    bool rising;
    bool falling;
    uint64_t timestamp_ns;
};

class MultiGpioIn {
public:
    MultiGpioIn(const std::string& chipname,
              const std::vector<unsigned int>& offsets,
              const std::string& consumer);

    ~MultiGpioIn();

    MultiGpioIn(const MultiGpioIn&) = delete;
    MultiGpioIn& operator=(const MultiGpioIn&) = delete;

    MultiGpioIn(MultiGpioIn&&) noexcept;
    MultiGpioIn& operator=(MultiGpioIn&&) noexcept;

    std::vector<int> read_all();
    bool wait_for_event(int timeout_ms);
    std::vector<GpioEvent> read_events();

private:
    gpiod_chip* chip_ = nullptr;
    gpiod_line_request* request_ = nullptr;
    std::vector<unsigned int> offsets_;
};

}

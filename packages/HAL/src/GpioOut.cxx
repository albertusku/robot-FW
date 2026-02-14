#include <hal/GpioOut.hxx>
#include <stdexcept>

namespace hal {

GpioOut::GpioOut(const std::string& chipname,
                 unsigned int line_offset,
                 const std::string& consumer)
    : line_offset_(line_offset)
{
    chip_ = gpiod_chip_open(chipname.c_str());
    if (!chip_)
        throw std::runtime_error("No puedo abrir chip GPIO");

    gpiod_line_settings* settings = gpiod_line_settings_new();
    if (!settings)
        throw std::runtime_error("No puedo crear line settings");

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(
    settings,
    GPIOD_LINE_VALUE_INACTIVE
);

    gpiod_line_config* line_cfg = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(line_cfg, &line_offset_, 1, settings);

    gpiod_request_config* req_cfg = gpiod_request_config_new();
    gpiod_request_config_set_consumer(req_cfg, consumer.c_str());

    request_ = gpiod_chip_request_lines(chip_, req_cfg, line_cfg);
    if (!request_)
        throw std::runtime_error("No puedo solicitar linea GPIO");

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(line_cfg);
    gpiod_request_config_free(req_cfg);
}

GpioOut::~GpioOut() {
    if (request_)
        gpiod_line_request_release(request_);
    if (chip_)
        gpiod_chip_close(chip_);
}

GpioOut::GpioOut(GpioOut&& other) noexcept {
    chip_ = other.chip_;
    request_ = other.request_;
    line_offset_ = other.line_offset_;

    other.chip_ = nullptr;
    other.request_ = nullptr;
}

GpioOut& GpioOut::operator=(GpioOut&& other) noexcept {
    if (this != &other) {
        if (request_)
            gpiod_line_request_release(request_);
        if (chip_)
            gpiod_chip_close(chip_);

        chip_ = other.chip_;
        request_ = other.request_;
        line_offset_ = other.line_offset_;

        other.chip_ = nullptr;
        other.request_ = nullptr;
    }
    return *this;
}

void GpioOut::set(bool value) {
    if (!request_)
        throw std::runtime_error("GPIO no inicializado");

    if (gpiod_line_request_set_value(
    request_,
    line_offset_,
    value ? GPIOD_LINE_VALUE_ACTIVE
          : GPIOD_LINE_VALUE_INACTIVE) < 0)
        throw std::runtime_error("Error al escribir GPIO");
}

} // namespace hal

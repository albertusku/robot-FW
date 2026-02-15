#include <hal/GpioIn.hxx>
#include <stdexcept>

namespace hal {

GpioIn::GpioIn(const std::string& chipname,
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

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(settings,GPIOD_LINE_BIAS_PULL_UP);

    gpiod_line_config* line_cfg = gpiod_line_config_new();
    if (!line_cfg) {
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip_);
        throw std::runtime_error("No puedo crear line config");
    }
    if (gpiod_line_config_add_line_settings(
            line_cfg,
            &line_offset_,
            1,
            settings) < 0) {
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(line_cfg);
        gpiod_chip_close(chip_);
        throw std::runtime_error("Error configurando line settings");
    }

    gpiod_request_config* req_cfg = gpiod_request_config_new();
    if (!req_cfg) {
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(line_cfg);
        gpiod_chip_close(chip_);
        throw std::runtime_error("No puedo crear request config");
    }
    gpiod_request_config_set_consumer(req_cfg, consumer.c_str());

    request_ = gpiod_chip_request_lines(chip_, req_cfg, line_cfg);
    if (!request_)
        throw std::runtime_error("No puedo solicitar linea GPIO");

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(line_cfg);
    gpiod_request_config_free(req_cfg);
}

GpioIn::~GpioIn() {
    if (request_)
        gpiod_line_request_release(request_);
    if (chip_)
        gpiod_chip_close(chip_);
}

GpioIn::GpioIn(GpioIn&& other) noexcept {
    chip_ = other.chip_;
    request_ = other.request_;
    line_offset_ = other.line_offset_;

    other.chip_ = nullptr;
    other.request_ = nullptr;
}

GpioIn& GpioIn::operator=(GpioIn&& other) noexcept {
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

bool GpioIn::read_digital() {
    if (!request_)
        throw std::runtime_error("GPIO no inicializado");

    int value = gpiod_line_request_get_value(
        request_,
        line_offset_);

    if (value == GPIOD_LINE_VALUE_ERROR)
        throw std::runtime_error("Error al leer GPIO");

    return value == GPIOD_LINE_VALUE_ACTIVE;
}




} // namespace hal

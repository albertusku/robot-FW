#include <hal/MultiGpioIn.hxx>
#include <stdexcept>

namespace hal {

MultiGpioIn::MultiGpioIn(const std::string& chipname,
                         const std::vector<unsigned int>& offsets,
                         const std::string& consumer,
                         int bias = GPIOD_LINE_BIAS_PULL_UP)
    : offsets_(offsets)
{
    if (offsets_.empty())
        throw std::runtime_error("Lista de offsets vacia");

    chip_ = gpiod_chip_open(chipname.c_str());
    if (!chip_)
        throw std::runtime_error("No puedo abrir chip GPIO");

    gpiod_line_settings* settings = gpiod_line_settings_new();
    if (!settings) {
        gpiod_chip_close(chip_);
        throw std::runtime_error("No puedo crear line settings");
    }

    // Configuracion: INPUT + PULL_UP + EDGE_BOTH
    gpiod_line_settings_set_direction(
        settings,
        GPIOD_LINE_DIRECTION_INPUT);

    switch (bias)
    {
    case 1:
        gpiod_line_settings_set_bias(settings,GPIOD_LINE_BIAS_AS_IS);
        break;
    case 2:
        gpiod_line_settings_set_bias(settings,GPIOD_LINE_BIAS_UNKNOWN);
        break;
    case 3:
        gpiod_line_settings_set_bias(settings,GPIOD_LINE_BIAS_DISABLED);
        break;
    case 4:
        gpiod_line_settings_set_bias(settings,GPIOD_LINE_BIAS_PULL_UP);
        break;
    case 5:
        gpiod_line_settings_set_bias(settings,GPIOD_LINE_BIAS_PULL_DOWN);
        break;
    default:
        gpiod_line_settings_set_bias(settings,GPIOD_LINE_BIAS_PULL_UP);
        break;
    }

    gpiod_line_settings_set_edge_detection(
        settings,
        GPIOD_LINE_EDGE_BOTH);

    gpiod_line_config* line_cfg = gpiod_line_config_new();
    if (!line_cfg) {
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip_);
        throw std::runtime_error("No puedo crear line config");
    }

    if (gpiod_line_config_add_line_settings(
            line_cfg,
            offsets_.data(),
            offsets_.size(),
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

    gpiod_request_config_set_consumer(
        req_cfg,
        consumer.c_str());

    request_ = gpiod_chip_request_lines(
        chip_,
        req_cfg,
        line_cfg);

    if (!request_) {
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(line_cfg);
        gpiod_request_config_free(req_cfg);
        gpiod_chip_close(chip_);
        throw std::runtime_error("No puedo solicitar lineas GPIO");
    }

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(line_cfg);
    gpiod_request_config_free(req_cfg);
}

// ------------------------------

MultiGpioIn::~MultiGpioIn()
{
    if (request_)
        gpiod_line_request_release(request_);

    if (chip_)
        gpiod_chip_close(chip_);
}

// ------------------------------

MultiGpioIn::MultiGpioIn(MultiGpioIn&& other) noexcept
{
    chip_ = other.chip_;
    request_ = other.request_;
    offsets_ = std::move(other.offsets_);

    other.chip_ = nullptr;
    other.request_ = nullptr;
}

MultiGpioIn& MultiGpioIn::operator=(MultiGpioIn&& other) noexcept
{
    if (this != &other) {

        if (request_)
            gpiod_line_request_release(request_);

        if (chip_)
            gpiod_chip_close(chip_);

        chip_ = other.chip_;
        request_ = other.request_;
        offsets_ = std::move(other.offsets_);

        other.chip_ = nullptr;
        other.request_ = nullptr;
    }

    return *this;
}

// ------------------------------

std::vector<int> MultiGpioIn::read_all()
{
    if (!request_)
        throw std::runtime_error("GPIO no inicializado");

    std::vector<int> values(offsets_.size());

    for (size_t i = 0; i < offsets_.size(); ++i) {

        int val = gpiod_line_request_get_value(
            request_,
            offsets_[i]);

        if (val == GPIOD_LINE_VALUE_ERROR)
            throw std::runtime_error("Error leyendo GPIO");

        values[i] = val;
    }

    return values;
}

// ------------------------------

bool MultiGpioIn::wait_for_event(int timeout_ms)
{
    if (!request_)
        throw std::runtime_error("GPIO no inicializado");

    int ret = gpiod_line_request_wait_edge_events(
        request_,
        timeout_ms * 1000000LL);

    if (ret < 0)
        throw std::runtime_error("Error esperando evento");

    return ret > 0;
}

// ------------------------------

std::vector<GpioEvent> MultiGpioIn::read_events()
{
    if (!request_)
        throw std::runtime_error("GPIO no inicializado");

    std::vector<GpioEvent> events;

    gpiod_edge_event_buffer* buffer =
        gpiod_edge_event_buffer_new(16);

    if (!buffer)
        throw std::runtime_error("No se pudo crear buffer");

    int n = gpiod_line_request_read_edge_events(
        request_,
        buffer,
        16);

    if (n < 0) {
        gpiod_edge_event_buffer_free(buffer);
        throw std::runtime_error("Error leyendo eventos");
    }

    for (int i = 0; i < n; ++i) {

        gpiod_edge_event* ev =
            gpiod_edge_event_buffer_get_event(buffer, i);

        GpioEvent e;

        e.line_offset =
            gpiod_edge_event_get_line_offset(ev);

        int type =
            gpiod_edge_event_get_event_type(ev);

        e.rising =
            (type == GPIOD_EDGE_EVENT_RISING_EDGE);

        e.falling =
            (type == GPIOD_EDGE_EVENT_FALLING_EDGE);

        e.timestamp_ns =
            gpiod_edge_event_get_timestamp_ns(ev);

        events.push_back(e);
    }

    gpiod_edge_event_buffer_free(buffer);

    return events;
}

} // namespace hal

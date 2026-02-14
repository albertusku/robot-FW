#include <hal/LinuxPwm.hxx>
#include <fstream>
#include <stdexcept>
#include <thread>
#include <chrono>

namespace hal {

LinuxPwm::LinuxPwm(const std::string& pwmchip_path, int channel)
    : chip_(pwmchip_path), ch_(channel)
{
    pwm_path_ = chip_ + "/pwm" + std::to_string(ch_);

    if (!exists_file(pwm_path_ + "/enable")) {
        write_file(chip_ + "/export", std::to_string(ch_));

        // Esperar a que aparezca el directorio
        for (int i = 0; i < 50; ++i) {
            if (exists_file(pwm_path_ + "/enable"))
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    enable(false);
}

LinuxPwm::~LinuxPwm() {
    try {
        enable(false);
        write_file(chip_ + "/unexport", std::to_string(ch_));
    } catch (...) {
        // Nunca lanzar excepcion desde destructor
    }
}

LinuxPwm::LinuxPwm(LinuxPwm&& other) noexcept
{
    chip_ = std::move(other.chip_);
    ch_ = other.ch_;
    pwm_path_ = std::move(other.pwm_path_);
    period_ns_ = other.period_ns_;

    other.period_ns_ = -1;
}

LinuxPwm& LinuxPwm::operator=(LinuxPwm&& other) noexcept
{
    if (this != &other) {
        chip_ = std::move(other.chip_);
        ch_ = other.ch_;
        pwm_path_ = std::move(other.pwm_path_);
        period_ns_ = other.period_ns_;

        other.period_ns_ = -1;
    }
    return *this;
}

void LinuxPwm::set_frequency_hz(int freq_hz) {
    if (freq_hz <= 0)
        throw std::invalid_argument("freq_hz debe ser > 0");

    period_ns_ = static_cast<long long>(1'000'000'000LL / freq_hz);
    write_file(pwm_path_ + "/period", std::to_string(period_ns_));
}

void LinuxPwm::set_duty_cycle(float duty_0_1) {
    if (period_ns_ <= 0)
        throw std::runtime_error("Debes llamar antes a set_frequency_hz()");

    if (duty_0_1 < 0.0f) duty_0_1 = 0.0f;
    if (duty_0_1 > 1.0f) duty_0_1 = 1.0f;

    long long duty_ns = static_cast<long long>(period_ns_ * duty_0_1);
    write_file(pwm_path_ + "/duty_cycle", std::to_string(duty_ns));
}

void LinuxPwm::enable(bool en) {
    write_file(pwm_path_ + "/enable", en ? "1" : "0");
}

void LinuxPwm::write_file(const std::string& path,
                          const std::string& value)
{
    std::ofstream f(path);
    if (!f)
        throw std::runtime_error("No puedo abrir: " + path);

    f << value;
    if (!f)
        throw std::runtime_error("No puedo escribir en: " + path);
}

bool LinuxPwm::exists_file(const std::string& path) {
    std::ifstream f(path);
    return static_cast<bool>(f);
}

} // namespace hal

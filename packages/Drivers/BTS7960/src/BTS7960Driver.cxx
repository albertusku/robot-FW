#include <drivers/BTS7960Driver.hxx>

#include <algorithm>
#include <cmath>

namespace drivers {

BTS7960Driver::BTS7960Driver(hal::GpioOut r_en,
                             hal::GpioOut l_en,
                             hal::LinuxPwm rpwm,
                             hal::LinuxPwm lpwm,
                             int pwm_freq_hz)
    : r_en_(std::move(r_en))
    , l_en_(std::move(l_en))
    , rpwm_(std::move(rpwm))
    , lpwm_(std::move(lpwm))
{
    r_en_.set(true);
    l_en_.set(true);

    rpwm_.set_frequency_hz(pwm_freq_hz);
    lpwm_.set_frequency_hz(pwm_freq_hz);

    rpwm_.set_duty_cycle(0.0f);
    lpwm_.set_duty_cycle(0.0f);

    rpwm_.enable(true);
    lpwm_.enable(true);
}

void BTS7960Driver::set_speed(float speed) {
    speed = std::clamp(speed, -1.0f, 1.0f);
    const float duty = std::fabs(speed);

    if (speed > 0.0f) {
        rpwm_.set_duty_cycle(duty);
        lpwm_.set_duty_cycle(0.0f);
    }
    else if (speed < 0.0f) {
        rpwm_.set_duty_cycle(0.0f);
        lpwm_.set_duty_cycle(duty);
    }
    else {
        rpwm_.set_duty_cycle(0.0f);
        lpwm_.set_duty_cycle(0.0f);
    }
}

void BTS7960Driver::stop() {
    rpwm_.set_duty_cycle(0.0f);
    lpwm_.set_duty_cycle(0.0f);
}

} // namespace drivers

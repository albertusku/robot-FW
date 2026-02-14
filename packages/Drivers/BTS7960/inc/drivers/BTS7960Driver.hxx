#pragma once

#include <motordc/IMotorDriver.hxx>   // interfaz base
#include <hal/GpioOut.hxx>
#include <hal/LinuxPwm.hxx>

namespace drivers {

class BTS7960Driver : public motordc::IMotorDriver {
public:
    BTS7960Driver(hal::GpioOut r_en,
                  hal::GpioOut l_en,
                  hal::LinuxPwm rpwm,
                  hal::LinuxPwm lpwm,
                  int pwm_freq_hz = 20000);

    void set_speed(float speed) override;
    void stop() override;

private:
    hal::GpioOut r_en_;
    hal::GpioOut l_en_;
    hal::LinuxPwm rpwm_;
    hal::LinuxPwm lpwm_;
};

} // namespace drivers

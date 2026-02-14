#pragma once

#include <string>

namespace hal {

class LinuxPwm {
public:
    LinuxPwm(const std::string& pwmchip_path, int channel);
    ~LinuxPwm();

    // No copiables
    LinuxPwm(LinuxPwm&& other) noexcept;
    LinuxPwm& operator=(LinuxPwm&& other) noexcept;


    void set_frequency_hz(int freq_hz);
    void set_duty_cycle(float duty_0_1);
    void enable(bool en);

private:
    void write_file(const std::string& path, const std::string& value);
    bool exists_file(const std::string& path);

    std::string chip_;
    int ch_;
    std::string pwm_path_;
    long long period_ns_ = -1;
};

} // namespace hal

#include <hal/GpioOut.hxx>
#include <hal/LinuxPwm.hxx>
#include <drivers/BTS7960Driver.hxx>

int main() {
    try {
        hal::GpioOut r_en("gpiochip0", 17, "BTS7960_R_EN");
        hal::GpioOut l_en("gpiochip0", 27, "BTS7960_L_EN");
        hal::LinuxPwm rpwm("/sys/class/pwm/pwmchip0", 0, "BTS7960_R_PWM");
        hal::LinuxPwm lpwm("/sys/class/pwm/pwmchip0", 1, "BTS7960_L_PWM");

        drivers::BTS7960Driver motor(r_en, l_en, rpwm, lpwm);

        motor.set_speed(0.5); // Avanza a mitad de velocidad
        std::this_thread::sleep_for(std::chrono::seconds(2));

        motor.set_speed(-0.5); // Retrocede a mitad de velocidad
        std::this_thread::sleep_for(std::chrono::seconds(2));

        motor.stop(); // Detiene el motor
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
#pragma once

namespace motordc {

class IMotorDriver {
public:
    virtual ~IMotorDriver() = default;

    // Velocidad normalizada:
    //   -1.0  → giro maximo sentido negativo
    //    0.0  → parado
    //   +1.0  → giro maximo sentido positivo
    virtual void set_speed(float speed) = 0;

    // Parada segura (puede ser override de set_speed(0))
    virtual void stop() = 0;
};

} // namespace motordc

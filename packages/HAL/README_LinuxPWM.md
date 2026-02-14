# LinuxPwm -- Control de PWM hardware (Linux sysfs)

## Descripción

`hal::LinuxPwm` encapsula el control de un canal PWM hardware expuesto
por Linux a través de:

    /sys/class/pwm/pwmchipX/pwmY/

La clase se encarga de:

-   Exportar el canal PWM si no está creado
-   Configurar frecuencia (periodo) y duty cycle
-   Habilitar/deshabilitar la salida
-   Liberar el canal automáticamente al destruir el objeto (RAII)

Cada objeto `LinuxPwm` representa **un único canal PWM** (por ejemplo
`pwmchip0` canal `0`).

------------------------------------------------------------------------

## Constructor

``` cpp
LinuxPwm(const std::string& pwmchip_path, int channel);
```

------------------------------------------------------------------------

## Parámetros del constructor

### 1️⃣ pwmchip_path

Ruta al controlador PWM (chip PWM) en sysfs.

Ejemplo típico:

    /sys/class/pwm/pwmchip0

### Cómo encontrarlo

Listar controladores PWM disponibles:

``` bash
ls /sys/class/pwm/
```

Ejemplo de salida:

    pwmchip0

Entonces el `pwmchip_path` sería:

``` cpp
"/sys/class/pwm/pwmchip0"
```

------------------------------------------------------------------------

### 2️⃣ channel

Número de canal dentro del `pwmchip`.

Los canales válidos van desde:

    0  hasta  (npwm - 1)

### Cómo saber cuántos canales hay

``` bash
cat /sys/class/pwm/pwmchip0/npwm
```

Ejemplo de salida:

    2


------------------------------------------------------------------------

## Ejemplo de creación

``` cpp
hal::LinuxPwm rpwm("/sys/class/pwm/pwmchip0", 0);
```

------------------------------------------------------------------------

## Configuración típica (frecuencia + duty + enable)

### 1) Fijar frecuencia

``` cpp
rpwm.set_frequency_hz(20000); // 20 kHz
```

Esto configura internamente el fichero:

    .../period

El periodo se calcula como:

    period_ns = 1e9 / freq_hz

Ejemplo: - 20 kHz -\> 50_000 ns

------------------------------------------------------------------------

### 2) Fijar duty cycle

``` cpp
rpwm.set_duty_cycle(0.60f); // 60%
```

El duty se expresa en rango:

-   `0.0` -\> 0%
-   `1.0` -\> 100%

Internamente se convierte a nanosegundos:

    duty_ns = period_ns * duty

y se escribe en:

    .../duty_cycle

------------------------------------------------------------------------

### 3) Activar o desactivar el PWM

``` cpp
rpwm.enable(true);   // start PWM
rpwm.enable(false);  // stop PWM
```

Esto escribe en:

    .../enable

------------------------------------------------------------------------

## Qué hace internamente el constructor

Al construir:

1.  Comprueba si existe el canal (`.../pwmX/enable`)

2.  Si no existe, exporta el canal escribiendo en:

        pwmchipX/export

3.  Espera a que Linux cree el directorio `pwmX/`

4.  Deja el PWM deshabilitado por seguridad

------------------------------------------------------------------------

## Cómo validar manualmente que el canal existe

Si quieres comprobarlo sin código:

1)  Ver contenido del pwmchip:

``` bash
ls /sys/class/pwm/pwmchip0/
```

2)  Exportar canal 0 manualmente:

``` bash
echo 0 | sudo tee /sys/class/pwm/pwmchip0/export
```

3)  Ver que aparece:

``` bash
ls /sys/class/pwm/pwmchip0/pwm0/
```

Deberías ver ficheros como:

-   `period`
-   `duty_cycle`
-   `enable`
-   `polarity`

------------------------------------------------------------------------

## Notas importantes

-   `LinuxPwm` controla un canal PWM, pero eso no garantiza que el PWM
    salga por cualquier GPIO. Para que haya señal física, el pin debe
    estar mapeado a PWM hardware y configurado en su función
    alternativa.
-   La clase no es copiable (ownership exclusivo).
-   La clase es movible.

------------------------------------------------------------------------

## Resumen

`LinuxPwm` representa:

> Un canal PWM hardware controlado desde sysfs.

Para usarlo correctamente necesitas:

1.  Saber qué `pwmchip` hay en `/sys/class/pwm/`
2.  Saber cuántos canales tiene (`npwm`) y elegir `channel`
3.  Configurar frecuencia (`set_frequency_hz`)
4.  Configurar duty (`set_duty_cycle`)
5.  Activar (`enable(true)`)
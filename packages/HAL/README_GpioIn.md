# GpioIn

## Descripción

`GpioIn` es una clase de la capa HAL diseñada para gestionar un único
GPIO configurado como entrada utilizando la API moderna de `libgpiod`
(v2).

Permite:

-   Leer el estado digital de un GPIO.
-   Configurar bias (pull-up, pull-down o floating).
-   Usarse como bloque base para sensores digitales simples.

------------------------------------------------------------------------

## Constructor

``` cpp
GpioIn(const std::string& chipname,
       unsigned int line_offset,
       const std::string& consumer);
```

### Parámetros

### 1. `chipname`

Nombre del chip GPIO.

En Raspberry Pi 5 normalmente:

``` bash
gpioinfo
```

Salida típica:

    gpiochip0

Uso en código:

``` cpp
"/dev/gpiochip0"
```

------------------------------------------------------------------------

### 2. `line_offset`

Número de línea dentro del chip.

Para descubrirlo:

``` bash
gpioinfo gpiochip0
```

Buscar líneas como:

    line 17: "GPIO17"
    line 23: "GPIO23"

Ejemplo:

``` cpp
17
```

------------------------------------------------------------------------

### 3. `consumer`

Nombre identificativo del proceso que usa la línea.

Se puede verificar con:

``` bash
gpioinfo
```

Ejemplo:

``` cpp
"limit_switch"
```

------------------------------------------------------------------------

## Configuración interna

Actualmente la clase configura:

-   Dirección: `INPUT`
-   Bias: `PULL_UP` (configurable si se modifica la implementación)
-   Sin detección de flancos (lectura directa)

------------------------------------------------------------------------

## Método disponible

### `read_digital()`

``` cpp
bool read_digital();
```

Devuelve:

-   `true` si la línea está en estado activo.
-   `false` si está en estado inactivo.

Ejemplo:

``` cpp
hal::GpioIn button("/dev/gpiochip0", 17, "button_input");

if (button.read_digital()) {
    // estado activo
}
```

Internamente compara el valor leído con:

``` cpp
GPIOD_LINE_VALUE_ACTIVE
```

Esto hace la interfaz más limpia que devolver directamente el valor
entero de la librería.

------------------------------------------------------------------------

## Consideraciones importantes

-   Requiere permisos para acceder a `/dev/gpiochipX`.
-   Requiere `libgpiod` v2 instalada.

------------------------------------------------------------------------

## Dependencias

-   Linux con soporte GPIO
-   `libgpiod` v2
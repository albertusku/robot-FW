# MultiGpioIn

## Descripción

`MultiGpioIn` es una clase de la capa HAL que permite gestionar
múltiples GPIO como entradas utilizando la API moderna de `libgpiod`
(v2).

Permite:

-   Leer varios GPIO simultáneamente.
-   Esperar eventos de flanco (rising/falling).
-   Obtener eventos con timestamp preciso generado por el kernel.
-   Utilizar un único request para varias líneas (importante para
    coherencia temporal).

------------------------------------------------------------------------

## Constructor

``` cpp
MultiGpioIn(const std::string& chipname,
            const std::vector<unsigned int>& offsets,
            const std::string& consumer);
```

### Parámetros

### 1. `chipname`

Nombre del chip GPIO.

Ejemplo típico en Raspberry Pi 5:

``` bash
gpioinfo
```

Salida habitual:

    gpiochip0

Uso en código:

``` cpp
"gpiochip0"
```

------------------------------------------------------------------------

### 2. `offsets`

Vector con los números de línea (offset) dentro del chip.

Ejemplo:

``` cpp
{17, 18}
```

Para descubrir los offsets disponibles:

``` bash
gpioinfo gpiochip0
```

Buscar líneas como:

    line 17: "GPIO17"
    line 18: "GPIO18"

------------------------------------------------------------------------

### 3. `consumer`

Nombre identificativo del proceso que usa los GPIO.

Se puede ver con:

``` bash
gpioinfo
```

Ejemplo:

``` cpp
"motor_encoder"
```

------------------------------------------------------------------------

## Configuración interna

Actualmente la clase configura:

-   Dirección: `INPUT`
-   Bias: `PULL_UP`
-   Edge detection: `BOTH`

Esto permite usarla tanto para lectura digital como para detección de
flancos.

------------------------------------------------------------------------

## Métodos disponibles

### 1. `read_all()`

``` cpp
std::vector<int> read_all();
```

Devuelve el estado actual de todas las líneas en el mismo orden en que
fueron pasadas al constructor.

Ejemplo:

``` cpp
auto values = gpio.read_all();
int A = values[0];
int B = values[1];
```

------------------------------------------------------------------------

### 2. `wait_for_event(int timeout_ms)`

``` cpp
bool wait_for_event(int timeout_ms);
```

Bloquea el hilo hasta que ocurra un evento o expire el timeout.

-   Devuelve `true` si ocurrió un evento.
-   Devuelve `false` si expiró el timeout.

Ejemplo:

``` cpp
if (gpio.wait_for_event(100)) {
    // hubo evento
}
```

------------------------------------------------------------------------

### 3. `read_events()`

``` cpp
std::vector<GpioEvent> read_events();
```

Lee todos los eventos pendientes del kernel.

Cada `GpioEvent` contiene:

``` cpp
struct GpioEvent {
    unsigned int line_offset;
    bool rising;
    bool falling;
    uint64_t timestamp_ns;
};
```

Permite:

-   Detectar qué línea cambió.
-   Saber si fue flanco ascendente o descendente.
-   Obtener timestamp preciso en nanosegundos.

Uso típico:

``` cpp
auto events = gpio.read_events();

for (const auto& ev : events) {
    if (ev.rising) {
        // flanco ascendente
    }
}
```

------------------------------------------------------------------------

## Consideraciones importantes

-   Los eventos se entregan en orden temporal correcto.
-   El timestamp proviene del kernel (más preciso que std::chrono).
-   No copiar la clase (copy constructor eliminado).
-   Sí es movible (move semantics implementadas).
-   Requiere `libgpiod` instalada en el sistema.

------------------------------------------------------------------------

## Dependencias

-   Linux con soporte GPIO vía `/dev/gpiochipX`
-   `libgpiod` v2
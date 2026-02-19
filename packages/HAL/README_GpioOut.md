Con esta clase controlarmos un gpio en concreto
Contructor--->3 parametros:
1. chipname: Controlador GPIO que se va a usar ("gpiodetect")
2. line_offset: gpio que se va a utilizar ("gpioinfo")
3. consumer: identificador

# GpioOut -- Control de una línea GPIO (libgpiod v2)

## Descripción

`hal::GpioOut` encapsula el control exclusivo de una línea GPIO
configurada como **salida** utilizando la API **libgpiod v2**.

La clase se encarga de:

-   Abrir el controlador GPIO (`gpiochipX`)
-   Reservar una línea concreta
-   Configurarla como salida
-   Definir un estado inicial seguro (LOW)


Cada objeto `GpioOut` representa **un único GPIO concreto**.

------------------------------------------------------------------------

## Constructor

``` cpp
GpioOut(const std::string& chipname,
        unsigned int line_offset,
        const std::string& consumer);
```

------------------------------------------------------------------------

## Parámetros del constructor

### 1️⃣ chipname

Nombre del controlador GPIO.

Ejemplo típico en Raspberry Pi 5:

    gpiochip0

Corresponde al dispositivo:

    /dev/gpiochip0

### Cómo encontrarlo

``` bash
gpiodetect
```

Ejemplo de salida:

    gpiochip0 [pinctrl-rp1] (54 lines)

En la mayoría de casos, los GPIO del header de la Raspberry Pi están en
`gpiochip0`.

------------------------------------------------------------------------

### 2️⃣ line_offset

Número de línea dentro del chip GPIO.

⚠️ No es el número físico del pin.\
Es el número que reporta el kernel.

Ejemplo de salida de `gpioinfo`:

    line 23: "GPIO23" input

Entonces:

``` cpp
line_offset = 23;
```

### Cómo encontrarlo

``` bash
gpioinfo
```

Busca la línea correspondiente al GPIO que quieres usar.

------------------------------------------------------------------------

### 3️⃣ consumer

Nombre descriptivo para identificar quién usa la línea.

No afecta al comportamiento eléctrico.\
Se usa para depuración y diagnóstico.

Ejemplo:

``` cpp
"bts_r_en"
```

Si ejecutas:

``` bash
gpioinfo
```

Podrás ver:

    line 23: "GPIO23" output consumer="bts_r_en"

------------------------------------------------------------------------

## Ejemplo de uso

``` cpp
hal::GpioOut r_en("/dev/gpiochip0", 23, "bts_r_en");

r_en.set(true);   // HIGH
r_en.set(false);  // LOW
```

------------------------------------------------------------------------

## Qué hace internamente

Al construirse:

1.  Abre el dispositivo `/dev/gpiochipX`
2.  Crea configuración de línea
3.  Establece dirección OUTPUT
4.  Define valor inicial LOW
5.  Solicita control exclusivo al kernel

Al destruirse:

-   Libera automáticamente la línea GPIO

------------------------------------------------------------------------

## Configuración actual por defecto

La implementación actual configura:

-   Dirección: OUTPUT
-   Valor inicial: INACTIVE (LOW)
-   Sin bias (pull-up / pull-down)
-   Sin active-low
-   Sin edge detection

------------------------------------------------------------------------


------------------------------------------------------------------------

## Notas importantes

-   Solo un proceso puede reservar una línea GPIO al mismo tiempo.
-   Si intentas crear dos `GpioOut` usando el mismo `line_offset`, la
    segunda construcción fallará.
-   La clase no es copiable (ownership exclusivo).
-   La clase es movible.

------------------------------------------------------------------------

## Resumen

`GpioOut` representa:

> Una línea GPIO concreta configurada como salida y bajo control
> exclusivo del proceso.

Para usarla correctamente necesitas:

1.  Saber qué `gpiochip` usar (`gpiodetect`)
2.  Saber el `line_offset` (`gpioinfo`)
3.  Definir un nombre `consumer` descriptivo
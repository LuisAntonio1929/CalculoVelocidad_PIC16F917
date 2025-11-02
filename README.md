# CalculoVelocidad_PIC16F917
Este es un proyecto simple para calcular la velocidad de un motor a partir de su encoder usando un Pic16f917.

## Cálculos previos

### Cálculo del TMR1
Se tienen los siguientes datos:

- Periodo de la interrupción: **T = 100 ms**
- Frecuencia del oscilador interno: **$F_{OSC} = 8 \text{ MHz}$**
- Prescaler: **1:4**

Aplicando la fórmula del TMR1 se tiene:

$$
T = \frac{4 \cdot \text{PRESCALER}}{F_{OSC}} \cdot (65535 - TMR1)
$$

Reemplazando los datos, el número con el que se tiene que cargar al registro es:

$$
TMR1 = 15535
$$

---

### Cálculo de la velocidad

Datos conocidos:

- Tiempo de muestreo: **$T_m = 100 \text{ ms}$**
- Se desea obtener la velocidad angular en **RPM**
- Número de pulsos por vuelta: **100**
- Número de pulsos ($N_{pulsos}$) contados se almacenan en el TMR0

Número de vueltas:

$$
N_{vueltas} = \frac{N_{pulsos}}{100}
$$
Velocidad en RPM:

$$
V_{motor} = \frac{N_{vueltas}}{T_m} = \frac{N_{pulsos}}{100 \cdot 100 \text{ ms}} = \frac{N_{pulsos}}{10 \text{ s} \cdot \frac{1 \text{ min}}{60 \text{ s}}} = N_{pulsos} \cdot 6
$$

---

## Pseudocódigo

```c
// Configuración de los fusibles
Delay con oscilador interno ← 8MHz
MCLR activado
WDT desactivado
Protección de código desactivada
Brown-out desactivado

// Configuración del oscilador
Interno de alta frecuencia
Fosc ← 8MHz

// Configuración de los pines
RD0 (LED1) y RD1 (LED2) → salidas
RB0 (PULS) y RA4 (TOCKI) → entradas
LED1 ← 0
LED2 ← 0

// Configuración del TMR0
Modo ← contador externo (RA4)
Prescaler ← 1
TMR0 ← 0

// Configuración del TMR1
Fuente ← Fosc/4
Prescaler ← 4
TMR1 ← 15535

// Configuración de las interrupciones
INTCON.GIE ← 1      // Global interrupt enable
INTCON.PEIE ← 1     // Peripheral interrupt enable
INTCON.TMR0IE ← 1   // TMR0 interrupt enable
INTCON.INT0IE ← 1   // External interrupt enable
PIE1.TMR1IE ← 1     // TMR1 interrupt enable

// Bucle principal
Hacer nada.

// Rutinas de interrupción

// INTERRUPCIÓN EXTERNA (RB0)
Esperar 20 ms (antirrebote)
Si RB0 = 0:
    Alternar LED1
Limpiar bandera INT

// INTERRUPCIÓN TMR0
pulsos ← pulsos + 256
Limpiar bandera TMR0

// INTERRUPCIÓN TMR1
velocidad ← (pulsos + TMR0) × 6
Si velocidad > 800:
    LED2 ← 1
Sino:
    LED2 ← 0
pulsos ← 0
TMR0 ← 0
Recargar TMR1 ← 15535
Limpiar bandera TMR1
```
## Código Source
```
#include <16F917.h>
#device *=16
#device adc = 8
//Configuracion de los fusibles del microcontrolador
#FUSES NOWDT
#FUSES INTRC_IO
#FUSES NOPUT
#FUSES NOPROTECT
#FUSES MCLR
#FUSES NOCPD
#FUSES NOBROWNOUT
#FUSES IESO
#FUSES FCMEN
#FUSES NODEBUG

#use delay(clock = 8M)

#BYTE PORTD = 0x08
#BYTE PORTB = 0x06
#BYTE PORTA = 0x05
#BYTE TRISD = 0x88
#BYTE TRISB = 0x86
#BYTE TRISA = 0x85

#BYTE OPTION_REG = 0x181
#BYTE INTCON = 0x8B
#BYTE PIE1 = 0x8C
#BYTE PIR1 = 0x0C
#BYTE T1CON = 0x10
#BYTE TMR1L = 0x0E
#BYTE TMR1H = 0x0F
#BYTE TMR0 = 0x01

#BYTE OSCCON = 0x8F
```
## Código Main

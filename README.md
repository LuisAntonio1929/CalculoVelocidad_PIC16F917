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

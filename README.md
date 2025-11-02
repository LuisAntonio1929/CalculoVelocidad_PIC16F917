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
```c
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
```c
#include <16F917.h>
#include "source.h"

unsigned int valorh, valorl;
unsigned int16 pulsos=0, velocidad;


#int_EXT
void EXT_isr() {
   delay_ms(20);
   if(bit_test(PORTB,0)==0){
      output_toggle(PIN_D0);
   }
   bit_clear(INTCON,1);		//Limpiar flag
}


#int_TIMER0
void TIMER0_isr() {
   pulsos += 256;
   bit_clear(INTCON,2);		//Limpiar flag
}

#int_TIMER1
void TIMER1_isr() {
   velocidad = (pulsos + TMR0)*6;
   if(velocidad > 800){
      bit_set(PORTD,1);
      }else{
      bit_clear(PORTD,1);
      }
   TMR0 = 0;
   pulsos = 0;
   TMR1H = valorh;
   TMR1L = valorl;
   bit_clear(PIR1,0);		//Limpiar flag
}

int main(){
   //Configuracion del oscilador
   bit_set(OSCCON,6);bit_set(OSCCON,5);bit_set(OSCCON,4);	//8 MHz
   bit_clear(OSCCON,3);		//Device is running from the internal system clock
   bit_set(OSCCON,2);		//HFINTOSC is stable
   bit_set(OSCCON,0);		//Internal oscillator is used for system clock
   //Configuracion de los pines
   bit_clear(TRISD,0);		//Pin RD0 como salida
   bit_clear(TRISD,1);		//Pin RD1 como salida
   bit_set(TRISB,0);		//Pin RB0 como entrada
   bit_set(TRISA,4);		//Pin RA4 como entrada
   output_low(PIN_D0);		//Pin RD0 en 0
   output_low(PIN_D1);		//Pin RD1 en 0
   //Configuracion de las interrupciones
   bit_clear(OPTION_REG,6);	//Interrupt on falling edge of RB0/INT/SEG0 pin
   bit_set(OPTION_REG,5);	//Transition on RA4/C1OUT/T0CKI/SEG4 pin
   bit_set(OPTION_REG,4);	//Increment on high-to-low transition on RA4/C1OUT/T0CKI/SEG4 pin
   bit_set(OPTION_REG,1);	//Prescaler is assigned to the WDT
   
   bit_set(INTCON,7);		//Enables all unmasked interrupts
   bit_set(INTCON,6);		//Enables all unmasked peripheral interrupts
   bit_set(INTCON,5);		//Enables the TMR0 interrupt
   bit_set(INTCON,4);		//Enables the RB0/INT/SEG0 external interrupt
   
   bit_set(PIE1,0);		//TMR1 Overflow Interrupt Enable
   //1:4 Prescale Value
   bit_set(T1CON,5);bit_clear(T1CON,4);
   bit_clear(T1CON,1);		//Internal clock (F OSC/4)
   //Para alcanzar una interrupcion cada 100ms = 4*4*(65535-TMR1)/(8MHz)
   //Se llega a que el valor de TMR1 = 15535
   valorh = 15535 >> 8;
   valorl = 15535 & 0xFF;
   TMR1H = valorh;
   TMR1L = valorl;
   TMR0 = 0;
   bit_set(T1CON,0);		//Timer1 On
   while(1);
   return 0;
}
```

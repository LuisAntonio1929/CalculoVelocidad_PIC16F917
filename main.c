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
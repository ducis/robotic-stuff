#include<avr/io.h>
#include<avr/interrupt.h>
//#include<avr/wdt.h>
#include"io.h"
#include"pwm.h"
#include"encoder.h"
#include"timer0.h"
#include"uart.h"
#include"pid.h"
#include"watchdog.h"
#include"spi.h"
#include"userinterface.h"
//#include"adc.h"

#define EnMotor PORTD|=(1<<PD6)
#define DisMotor PORTD&=~(1<<PD6)
int main()
{
   IO_Init();
   PWM_Init();
   UART_Init(Freq_CPU,BAUDRATE);
   while(1); //UART_Transmit(0x77);
   //Watchdog_Init();
   Timer0_Init();
   Encoder_Init();
   //ADC_Init();
   PID_Init();
   SPI_SlaveInit();
   EnMotor;
   sei();
   //Set_PWM(180);
   //while(1) Watchdog_Clear();
   while(1);
   return 0;
}

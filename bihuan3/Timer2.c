#include<avr/io.h>
#include<avr/interrupt.h>
#include"Timer2.h"
#include"spi.h"

void Timer2_Start()    //¶¨Ê±200us
{
    TCCR2=0;
    TCNT2=155;   
	TIMSK|=(1<<TOIE2);        
    TCCR2|=(1<<CS21)|(1<<CS20);     //32·ÖÆµ        
}

ISR(TIMER2_OVF_vect)
{
    TCCR2=0;
    SPI_SlaveInit();
}

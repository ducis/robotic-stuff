#include<avr/io.h>
#include<avr/wdt.h>
#include"watchdog.h"


void Watchdog_Init()
{
    //WDTCR|=((1<<WDTOE)|(1<<WDE));
    //WDTCR=(1<<WDE);
	wdt_enable(WDTO_15MS);
}

void Watchdog_Clear()
{
    wdt_reset();
}

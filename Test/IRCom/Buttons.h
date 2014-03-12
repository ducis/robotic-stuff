#ifndef _BUTTONS_H
#define _BUTTONS_H

#include <avr/io.h>

#include "Basic.h"

#define S2 PIND6
#define S3 PIND3
#define S4 PIND5
#define S5 PIND4

BOOL ButtonIsDown(unsigned char button);

#endif


#include "index.h"

BOOL ButtonIsDown(unsigned char button){
	return !(PIND&_BV(button));
}

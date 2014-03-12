#include <avr/io.h>

#define NUM_POINTS 16
#define POINT_IS_WHITE_BIT 1
#define POINT_WAIT_BEFORE_AD 50//500
#define POINT_WAIT_AFTER_AD 10
#define SHIFT_REGISTER_CLK_SETUP 0
#define SHIFT_REGISTER_CLK_HOLD 0
#define DISPLAY_WAIT 1000//100
#define THRESHOLD_RATIO 60
#define FREQ_MODIFIER 66

#define SET_BIT_VALUE(byte,iBit,bValue) ((bValue)?((byte)|=_BV(iBit)):((byte)&=~_BV(iBit)))

unsigned char g_defaultThreshold = 20;

void Wait( unsigned n ){
	while( n ) --n;
}

struct LineState{
};

short ADProc();
//BEGIN PointState
struct PointState{
	unsigned char m_minimum;
	unsigned char m_maximum;
	unsigned char m_threshold;
	unsigned char m_rawValue;//ad ouput 
	unsigned char m_flags;
}g_points[NUM_POINTS];

//private:
void UpdateRawValue(struct PointState * pPoint)
{
	unsigned char id = pPoint - g_points;
	PORTA = (PORTA & 15) | (id<<4);
	PORTB &= ~_BV(PORTB0);
	Wait( POINT_WAIT_BEFORE_AD );
	pPoint->m_rawValue = ADProc();
	PORTB |= _BV(PORTB0);
	Wait( POINT_WAIT_AFTER_AD );
}
//public:
void UpdatePointState(struct PointState * pPoint)
{
	UpdateRawValue(pPoint);
	if(pPoint->m_rawValue>pPoint->m_threshold)//g_whiteThreshold)
		pPoint->m_flags |=POINT_IS_WHITE_BIT;
	else 
		pPoint->m_flags &=~POINT_IS_WHITE_BIT;
}

void UpdateMinMax(struct PointState *pPoint)
{
	UpdateRawValue(pPoint);
	if(pPoint->m_rawValue<pPoint->m_minimum)	pPoint->m_minimum = pPoint->m_rawValue;
	if(pPoint->m_rawValue>pPoint->m_maximum)	pPoint->m_maximum = pPoint->m_rawValue;
}

void ComputeThreshold(struct PointState *pPoint)
{
	pPoint->m_threshold = ((long)pPoint->m_minimum+(long)pPoint->m_maximum)*THRESHOLD_RATIO/255;
}

void UseDefaultThreshold(struct PointState *pPoint)
{
	pPoint->m_threshold = g_defaultThreshold;
}

//Ctor
void ConstructPointState(struct PointState *pPoint)
{
	pPoint->m_minimum = 255;
	pPoint->m_maximum = 0;
	pPoint->m_flags = 0;
	pPoint->m_threshold = g_defaultThreshold;
}

//END PointState;


void WriteByteToTheShiftRegister(unsigned char b){
	int i;
	for(i=0;i<8;++i){
		PORTB&=~_BV(PORTB1);
		SET_BIT_VALUE(PORTB,PORTB2,b&1);
		//Wait(SHIFT_REGISTER_CLK_SETUP);
		PORTB|=_BV(PORTB1);
		//Wait(SHIFT_REGISTER_CLK_HOLD);
		b>>=1;
	}
}
int main(){
	//sbi(TCCR0,FOC0);
	//TCCR0|=_BV(FOC0);
	{
		DDRB = _BV(DDB0)|_BV(DDB1)|_BV(DDB2)|_BV(DDB3);
		PORTB = _BV(PORTB0);

		TCCR0 = _BV(WGM01)|_BV(COM00)|_BV(CS01)|_BV(CS00);
		TCNT0 = 0;
		OCR0 = FREQ_MODIFIER;//62;
		TIMSK = 0;
		TIFR = 0;

		DDRA = _BV(DDA4)|_BV(DDA5)|_BV(DDA6)|_BV(DDA7);
		PORTA = 0;

		DDRC = 0;
		PORTC = 0;

		//AD Converter
		ADMUX = _BV(REFS0)|_BV(ADLAR);
		ADCSRA = _BV(ADEN);
	}
	enum{
		STATE_INIT,
		STATE_IDLE,
		STATE_CALIBERATING,
		STATE_NORMAL,		
		NUM_STATES
	}currentState = STATE_INIT;

	while(1){
		switch(currentState){
		case STATE_INIT:
			{
				int i;
				for(i=0; i<NUM_POINTS; ++i){
					ConstructPointState(g_points+i);
				}
				WriteByteToTheShiftRegister(0);
				WriteByteToTheShiftRegister(0);
				currentState = STATE_IDLE;
			}
			break;
		case STATE_IDLE:
			{
				if( !(PINC&_BV(PINC0)) ){//Press S1 to caliberate
					currentState = STATE_CALIBERATING;
				}
				if( !(PINC&_BV(PINC1)) ){//Press S2 to scan using default threshold
					int i;
					for( i=0; i<NUM_POINTS; ++i ){
						UseDefaultThreshold(g_points+i);
					}
					currentState = STATE_NORMAL;
				}
			}
			break;
		case STATE_CALIBERATING:
			{
				int i;
				for(i=0; i<NUM_POINTS; ++i){
					UpdateMinMax(g_points+i);
				}
				if(PINC&_BV(PINC0)){
					for(i=0; i<NUM_POINTS; ++i){
						ComputeThreshold(g_points+i);
					}
					currentState = STATE_NORMAL;
				}
			}
			break;
		case STATE_NORMAL:
			{
				int i;
				for(i=0; i<NUM_POINTS; ++i){
					UpdatePointState(g_points+i);
					if(i%4==3){
						unsigned char v = ( (1<<7) >> (i/4) );
						{
							signed char j;
							for(j=3;j>=0;--j) {
								if(g_points[i-j].m_flags & POINT_IS_WHITE_BIT)
								{
									v|=1<<j;
								}
							}
						}
						WriteByteToTheShiftRegister(0);
						WriteByteToTheShiftRegister(0);
						WriteByteToTheShiftRegister(v);
						Wait(DISPLAY_WAIT);
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

short ADProc(){
	ADCSRA |= _BV(ADSC);
	while(ADCSRA & _BV(ADSC))
		;
	return ADCH;
}

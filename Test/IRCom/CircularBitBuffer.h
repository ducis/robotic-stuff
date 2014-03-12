#ifndef _CIRCULARBITBUFFER_H
#define _CIRCULARBITBUFFER_H

#define CIRCULARBITBUFFER_SIZE 256

struct CircularBitBuffer{
	unsigned short m_l;
	unsigned short m_r;
	unsigned short m_n;
	unsigned char m_data[CIRCULARBITBUFFER_SIZE];
};
void CircularBitBuffer_Ctor(struct CircularBitBuffer *pThis);
unsigned short CircularBitBuffer_NumBits( const struct CircularBitBuffer *pThis );
unsigned char CircularBitBuffer_PopBit(struct CircularBitBuffer *pThis);
void CircularBitBuffer_PushBit(struct CircularBitBuffer *pThis, unsigned char bit);
void CircularBitBuffer_Clear(struct CircularBitBuffer *pThis);


#endif

#include "CircularBitBuffer.h"

void CircularBitBuffer_Ctor(struct CircularBitBuffer *pThis)
{
	pThis->m_l = 0;
	pThis->m_r = 0;
	pThis->m_n = 0;
}

unsigned short CircularBitBuffer_NumBits( const struct CircularBitBuffer *pThis )
{
	return pThis->m_n;
}

unsigned char CircularBitBuffer_PopBit(struct CircularBitBuffer *pThis)
{
	unsigned char ret = pThis->m_data[pThis->m_l>>3]>>(pThis->m_l&7);
	--pThis->m_n;
	++pThis->m_l;
	if((pThis->m_l>>3)==CIRCULARBITBUFFER_SIZE){
		pThis->m_l &= 7;
	}
	return ret;
}
void CircularBitBuffer_PushBit(struct CircularBitBuffer *pThis, unsigned char bit){
	pThis->m_data[pThis->m_r>>3] &= ~(1<<(pThis->m_r&7));
	pThis->m_data[pThis->m_r>>3] |= (bit&1)<<(pThis->m_r&7);
	++pThis->m_n;
	++pThis->m_r;
	if((pThis->m_r>>3)==CIRCULARBITBUFFER_SIZE){
		pThis->m_r &= 7;
	}
}
void CircularBitBuffer_Clear(struct CircularBitBuffer *pThis){
	CircularBitBuffer_Ctor(pThis);
}

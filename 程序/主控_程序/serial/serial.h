#ifndef __serial_h__
#define __serial_h__

#include"header.h"
#include"stc12c5axx.h"

/*����2�궨��*/
#define S2RI 0x01	//����2�����ж������־λ
#define S2TI 0x02	//����2�����ж������־λ
extern void UartInit();
extern void UART_2SendOneByte(uchar c);
extern void UART_1SendOneByte(uchar c);
#endif
#ifndef __mp3_h__
#define __mp3_h__

#include"header.h"
#include"main.h"
#include"serial.h"
/*MP3��æ��־λ��Ĭ�ϸߵ�ƽ��æʱ����͵�ƽ*/
sbit busy = P1^0;

extern void Uart_SendCMD(uchar CMD ,uchar feedback , uchar dath , uchar datl);
#endif
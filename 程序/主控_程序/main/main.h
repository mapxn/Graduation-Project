#ifndef __main_h__
#define __main_h__
#include"stc12c5axx.h"
#include"header.h"

/*MP3����������ͨ������*/
sbit Re_A = P1^4;  
sbit Re_B = P1^5;

/********************************************/
/* Re_A = 1; Re_B = 0; ʱѡ��Ϊ��������
/* Re_A = 1; Re_B = 1; ʱѡ��ΪMP3���� 
/********************************************/
#define En_Blu {Re_B = 0;Re_A = 1;}/*��������ͨ��*/
#define En_Mp3 {Re_B = 1;Re_A = 1;}/*����MP3ͨ��*/

static uchar Recv_buf[10] = {0} ;/*��������*/
static uchar Send_buf[10] = {0} ;/*��������*/


extern uchar mode;
extern uchar flag ;
#endif
#include"key.h"
void Scankey()
{
	if(Key_1 == 0)
	{
		delay10ms();
		if(Key_1 == 0)
		{
			mode++;
			if(mode > 1) 
				mode = 0;	
		}
		while(!Key_1);	
		if(mode==0)
		{
			Uart_SendCMD(0x0f,0x00,0x02,0x23);/*����Ϊѭ��ģʽ*/ 
			delay800ms();
			while(!busy);
		}
		if(mode==1)
		{
			Uart_SendCMD(0x0f,0x00,0x02,0x24);/*����Ϊ����ģʽ*/ 
			delay800ms();
			while(!busy);
		}
		flag = 0;
		Uart_SendYuYin(0xaa,0x00,0x00);//��ʼʶ��
	}
	
	if(Key_2 == 0)
	{
		delay10ms();
		if(Key_2 == 0)
		{
			mode = 2;
		}
		while(!Key_1);	
		Uart_SendCMD(0x0f,0x00,0x01,0xa8);/*����ʱ����*/ 
		delay800ms();
		
		while(!busy); 
	}
		
}
			
			
			
			
	
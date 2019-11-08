#include"stc12c5axx.h"
#include"intrins.h"
#include"delay.h"
#include"main.h"
#include"header.h"
#include"mp3.h"
#include"key.h"
#include"voice.h"
#include"blueteeth.h"
#include"serial.h"
#include"ds3231.h"

/*����ʶ�𷵻�ֵ��־λ��flag��ģʽ�л���־λ��mode*/
uchar flag ,mode;

/*****************������******************/
void main()
{
	uchar cache,poem,song,c;
	c=0;/*����Ѹ����*/
	poem = 17;/*��ʫ*/
	song = 41;/*����*/
	En_Mp3;
	busy = 1;   						/*MP3��æ��־λ�øߵ�ƽ*/
	mode = 0;
    UartInit();							/*���пڳ�ʼ��*/
	delay200ms();
	delay200ms();
	delay200ms();
	Uart_SendCMD(0x06,0x00,0x00,0x0e);	/*����MP3����Ϊ9*/
	delay50ms();
	Uart_SendCMD(0x0f,0x00,0x02,0x01);	/*��������*/
	Voice_Init();
	Uart_SendYuYin(0xa9,0x01,0x00);		/*����ʶ����*/
	delay800ms();
	while(!busy);
	Uart_SendYuYin(0xaa,0x00,0x00);		/*ʶ��ʼ*/
	while(1)
	{
		Scankey();
		switch(mode)
		{
			case 0:
					if(flag==1)
					{
						flag = 0; 
					
						if(Recv_buf[1]	== 0x00 )/*ʶ��Ϊ001��001�ţ����*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x04);/*���ѽ*/ 
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						
						if(Recv_buf[1]	== 0x01 )/*ʶ��Ϊ001��002�ţ����ʲô����*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x0c);/*�ҽ�СϪ*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						
						if(Recv_buf[1]	== 0x02 )/*ʶ��Ϊ001��003�ţ����������˭*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x0d);/*�ҵ�����������*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						
						if(Recv_buf[1]	== 0x03 )/*ʶ��Ϊ001��004�ţ�����˭*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x01);/*�������ܼҾ�����СϪ*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						
						if(Recv_buf[1]	== 0x04 )/*ʶ��Ϊ001��005�ţ���̫ɵ��*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x06);/*��˵ʲô��û���۾�������*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						
						if(Recv_buf[1]	== 0x05 )/*ʶ��Ϊ001��006�ţ�1+1���ڼ�*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x0e);/*һ��һ���ڶ�*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						
						if(Recv_buf[1]	== 0x06 )/*ʶ��Ϊ001��007�ţ��㼸����*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x05);/*�ҽ���һ����*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						
						if(Recv_buf[1]	== 0x07 )/*ʶ��Ϊ001��008�ţ����������*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x08);/*������֣�ݴ�ѧ����˹����ѧԺ*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x08 )/*ʶ��Ϊ001��009�ţ���ܴ���ѽ*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x0b);/*��Ҳ��ô����*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x09)/*ʶ��Ϊ001��010�ţ������Ӿ��*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x0a);/*�ҵ�Ȼ����Ӿ��*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x0a)/*ʶ��Ϊ001��011�ţ���Ҫ������*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x06);/*��˵ʲô��û���۾�������*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if((Recv_buf[1] == 0x0b)||(Recv_buf[1] == 37)||(Recv_buf[1] == 53))/*ʶ��Ϊ001��012�ţ����׸��*/
						{
							if(song>52)
								song=41;
							Uart_SendCMD(0x0f,0x00,0x02,song);/*����*/
							song++;
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1] == 38)/*ʶ��ΪѦ֮ǫ*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,50);/*��Ա*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1] == 39)/*ʶ��Ϊ������*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,44);/*���С��*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1] == 40)/*ʶ��Ϊ�ֿ���*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,42);/*С����*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1] == 41)/*ʶ��Ϊ���ٺ�*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,45);/*���*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 42 )/*ʶ��Ϊ�մ���*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,47);/*С���*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	==  43)/*ʶ��Ϊ����Ѹ*/
						{
							c++;
							if(c%2 == 1)
							{
								Uart_SendCMD(0x0f,0x00,0x02,41);/*��õ��*/
							}
							if(c%2 == 0)
							{
								Uart_SendCMD(0x0f,0x00,0x02,48);/*ʮ��*/
							}
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	==  44)/*ʶ��Ϊ���ٻ�����*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,51);/*���ٻ�����*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	==  45)/*ʶ��Ϊ�������ʹ*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,52);/*�������ʹ*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	==  46)/*ʶ��Ϊ��õ��*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,41);/*��õ��*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	==  47)/*ʶ��ΪС����*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,42);/*С����*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 48 )/*ʶ��Ϊʱ�䶼ȥ����*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,43);/*ʱ�䶼ȥ����*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 49 )/*ʶ��Ϊ���С��*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,44);/*���С��*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 50 )/*ʶ��Ϊ��Ա*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,50);/*��Ա*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 51 )/*ʶ��Ϊͯ��*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,46);/*ͯ��*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						
						if((Recv_buf[1] == 14)||(Recv_buf[1] == 15)||(Recv_buf[1] == 23))/*ʶ��Ϊ���ڼ�����*/
						{
							mode = 2;
						}
						
						
						
						if(Recv_buf[1]	== 0x0c)/*ʶ��Ϊ001��013�ţ�˵��С����*/
						{
							Uart_SendCMD(0x06,0x00,0x00,0x0a);
							delay50ms();
							Uart_SendCMD(0x0f,0x00,0x02,0x07);/*�õ�����*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x0d)/*ʶ��Ϊ001��014�ţ�˵�������*/
						{
							Uart_SendCMD(0x06,0x00,0x00,0x12);/*��������Ϊ20*/
							delay50ms();
							Uart_SendCMD(0x0f,0x00,0x02,0x07);/*�õ�����*/
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if((Recv_buf[1]	== 0x10)||(Recv_buf[1]	== 52))/*ʶ��Ϊ001��017�ţ����׹�ʫ*/
						{
							if(poem>34)
								poem = 17;
							Uart_SendCMD(0x0f,0x00,0x02,poem);/*����ʫ*/
							poem++;
							delay800ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						/**/
						if(Recv_buf[1]	== 0x11)/*ʶ��Ϊ001��018�ţ���̨��*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x03);/*̨���Ѵ�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x31,0x31,0x33);/*��̨��*/
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x12)/*ʶ��Ϊ001��019�ţ��򿪷���*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x01);/*�����Ѵ�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x31,0x31,0x32);
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x1b)/*ʶ��Ϊ001��028�ţ��򿪺��*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x0d);/*����Ѵ�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x31,0x31,0x38);/*�򿪺��*/
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x1d)/*ʶ��Ϊ001��030�ţ����̵�*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x0b);/*�̵��Ѵ�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x31,0x31,0x37);
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x1c)/*ʶ��Ϊ001��029�ţ�������*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x09);/*�õ�����*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x31,0x31,0x36);
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						
						if(Recv_buf[1]	== 0x24)/*ʶ��Ϊ001��037�ţ��򿪵���*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x05);/*�õ�����*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x31,0x31,0x34);
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x21)/*ʶ��Ϊ001��034�ţ�����ˮ��*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x07);/*��ˮ���Ѵ�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x31,0x31,0x35);
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						/**/
						if(Recv_buf[1]	== 0x1a)/*ʶ��Ϊ001��027�ţ��ر�̨��*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x04);/*̨���ѹر�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x30,0x31,0x33);/*�ر�̨��*/
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x19)/*ʶ��Ϊ001��026�ţ��رշ���*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x02);/*�����ѹر�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x30,0x31,0x32);
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x1e)/*ʶ��Ϊ001��031�ţ��رպ��*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x0e);/*����ѹر�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x30,0x31,0x38);/*�رպ��*/
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x20)/*ʶ��Ϊ001��033�ţ��ر��̵�*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x0c);/*�̵��ѹر�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x30,0x31,0x37);
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x1f)/*ʶ��Ϊ001��032�ţ��ر�����*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x0a);/*�����ѹر�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x30,0x31,0x36);
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						
						if(Recv_buf[1]	== 0x23)/*ʶ��Ϊ001��036�ţ��رյ���*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x06);/*�����ѹر�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x30,0x31,0x34);
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						if(Recv_buf[1]	== 0x22)/*ʶ��Ϊ001��035�ţ��ر���ˮ��*/
						{
							Uart_SendCMD(0x0f,0x00,0x05,0x08);/*��ˮ���ѹر�*/
							delay800ms();
							En_Blu;
							delay50ms();
							Uart_SendBlue(0x30,0x31,0x35);
							En_Mp3;
							delay50ms();
							while(!busy);
							flag = 0; 
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						else
						{
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}

					}break;
						
			case 1:/*����ģʽ*/
					if(flag==1)
					{
						flag = 0; 
					
						if(Recv_buf[1]	== 0x18 )/*ʶ��Ϊ�����ǵػ�*/
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x27);/*���������*/ 
							delay800ms();
							while(!busy);
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
							while(1)
							{
								if(flag==1)
								{
									flag = 0; 
								
									if(Recv_buf[1]	== 0x00 )/*ʶ��Ϊ001��001�ţ����*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x04);/*���ѽ*/ 
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									
									if(Recv_buf[1]	== 0x01 )/*ʶ��Ϊ001��002�ţ����ʲô����*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x0c);/*�ҽ�СϪ*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									
									if(Recv_buf[1]	== 0x02 )/*ʶ��Ϊ001��003�ţ����������˭*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x0d);/*�ҵ�����������*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									
									if(Recv_buf[1]	== 0x03 )/*ʶ��Ϊ001��004�ţ�����˭*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x01);/*�������ܼҾ�����СϪ*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									
									if(Recv_buf[1]	== 0x04 )/*ʶ��Ϊ001��005�ţ���̫ɵ��*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x06);/*��˵ʲô��û���۾�������*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									
									if(Recv_buf[1]	== 0x05 )/*ʶ��Ϊ001��006�ţ�1+1���ڼ�*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x0e);/*һ��һ���ڶ�*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									
									if(Recv_buf[1]	== 0x06 )/*ʶ��Ϊ001��007�ţ��㼸����*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x05);/*�ҽ���һ����*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									
									if(Recv_buf[1]	== 0x07 )/*ʶ��Ϊ001��008�ţ����������*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x08);/*������֣�ݴ�ѧ����˹����ѧԺ*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x08 )/*ʶ��Ϊ001��009�ţ���ܴ���ѽ*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x0b);/*��Ҳ��ô����*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x09)/*ʶ��Ϊ001��010�ţ������Ӿ��*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x0a);/*�ҵ�Ȼ����Ӿ��*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x0a)/*ʶ��Ϊ001��011�ţ���Ҫ������*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x06);/*�ҵ�Ȼ����Ӿ��*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x0b)/*ʶ��Ϊ001��012�ţ����׸��*/
									{
										Uart_SendCMD(0x0f,0x00,0x02,0x26);/*С�ݺ�*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x0c)/*ʶ��Ϊ001��013�ţ�˵��С����*/
									{
										Uart_SendCMD(0x06,0x00,0x00,0x0a);
										delay50ms();
										Uart_SendCMD(0x0f,0x00,0x02,0x07);/*�õ�����*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x0d)/*ʶ��Ϊ001��014�ţ�˵�������*/
									{
										Uart_SendCMD(0x06,0x00,0x00,0x12);/*��������Ϊ20*/
										delay50ms();
										Uart_SendCMD(0x0f,0x00,0x02,0x07);/*�õ�����*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x10)/*ʶ��Ϊ001��017�ţ�����ʫ��*/
									{
										poem++;
										if(poem>34)
											poem = 17;
										Uart_SendCMD(0x0f,0x00,0x02,poem);/*�õ�����*/
										delay800ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									/**/
									if(Recv_buf[1]	== 0x11)/*ʶ��Ϊ001��018�ţ���̨��*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x03);/*̨���Ѵ�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x31,0x31,0x33);/*��̨��*/
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x12)/*ʶ��Ϊ001��019�ţ��򿪷���*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x01);/*�����Ѵ�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x31,0x31,0x32);
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x1b)/*ʶ��Ϊ001��028�ţ��򿪺��*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x0d);/*����Ѵ�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x31,0x31,0x38);/*�򿪺��*/
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x1d)/*ʶ��Ϊ001��030�ţ����̵�*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x0b);/*�̵��Ѵ�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x31,0x31,0x37);
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x1c)/*ʶ��Ϊ001��029�ţ�������*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x09);/*�õ�����*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x31,0x31,0x36);
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									
									if(Recv_buf[1]	== 0x24)/*ʶ��Ϊ001��037�ţ��򿪵���*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x05);/*�õ�����*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x31,0x31,0x34);
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x21)/*ʶ��Ϊ001��034�ţ�����ˮ��*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x07);/*��ˮ���Ѵ�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x31,0x31,0x35);
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									/**/
									if(Recv_buf[1]	== 0x1a)/*ʶ��Ϊ001��027�ţ��ر�̨��*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x04);/*̨���ѹر�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x30,0x31,0x33);/*�ر�̨��*/
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x19)/*ʶ��Ϊ001��026�ţ��رշ���*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x02);/*�����ѹر�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x30,0x31,0x32);
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x1e)/*ʶ��Ϊ001��031�ţ��رպ��*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x0e);/*����ѹر�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x30,0x31,0x38);/*�رպ��*/
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x20)/*ʶ��Ϊ001��033�ţ��ر��̵�*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x0c);/*�̵��ѹر�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x30,0x31,0x37);
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x1f)/*ʶ��Ϊ001��032�ţ��ر�����*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x0a);/*�����ѹر�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x30,0x31,0x36);
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									
									if(Recv_buf[1]	== 0x23)/*ʶ��Ϊ001��036�ţ��رյ���*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x06);/*�����ѹر�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x30,0x31,0x34);
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
									if(Recv_buf[1]	== 0x22)/*ʶ��Ϊ001��035�ţ��ر���ˮ��*/
									{
										Uart_SendCMD(0x0f,0x00,0x05,0x08);/*��ˮ���ѹر�*/
										delay800ms();
										En_Blu;
										delay50ms();
										Uart_SendBlue(0x30,0x31,0x35);
										En_Mp3;
										delay50ms();
										while(!busy);
										flag = 0; 
										Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
										break;
									}
																	
									else
										break;

								}
							}
							
						}
						else
						{
							Uart_SendCMD(0x0f,0x00,0x02,0x28);/*��˵����ȷ����*/ 
							delay800ms();
							while(!busy);
							Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
						}
						
					}
					break;

			case 2:/*������ʱģʽ*/
				{
					cache = BCD2HEX(read_random(0x06));
					Uart_SendCMD(0x0f,0x00,0x06,cache);/*���*/
					delay800ms();
					cache = BCD2HEX(read_random(0x05));
					while(!busy);
			
					
					Uart_SendCMD(0x0f,0x00,0x08,cache);/*�·�*/
					delay800ms();
					cache = BCD2HEX(read_random(0x04));
					while(!busy);
			
					
					Uart_SendCMD(0x0f,0x00,0x09,cache);/*����*/
					delay800ms();
					cache = BCD2HEX(read_random(0x03));
					while(!busy);
			
					
					Uart_SendCMD(0x0f,0x00,0x07,cache);/*����*/
					delay800ms();
					cache = BCD2HEX(read_random(0x02));
					if(cache == 0)cache = 24;/*0�㼴��24��*/
					while(!busy);
			
					
					Uart_SendCMD(0x0f,0x00,0x0a,cache);/*Сʱ*/
					delay800ms();
					cache = BCD2HEX(read_random(0x01));
					if(cache == 0)cache = 60;/*��ּ�������*/
					while(!busy);
			
					
					Uart_SendCMD(0x0f,0x00,0x0b,cache);/*����*/
					delay800ms();
					while(!busy);
					
					Uart_SendCMD(0x0f,0x00,0x01,0xa9);/*�¶�*/
					delay800ms();
					cache = read_random(0x11);
					while(!busy);
					
					if(cache>127)
					{
						Uart_SendCMD(0x0f,0x00,0x01,0xb4);/*����*/
						delay800ms();
						while(!busy);
						Uart_SendCMD(0x0f,0x00,0x01,(~cache+1));/*�¶�*/
						delay800ms();
						while(!busy);
					}
					else
					{
						Uart_SendCMD(0x0f,0x00,0x01,cache);/*�¶�*/
						delay800ms();
						while(!busy);
					}
					
					Uart_SendCMD(0x0f,0x00,0x01,0xaa);/*���϶�*/
					delay800ms();
					while(!busy);
					
					
					Uart_SendYuYin(0xaa,0x00,0x00);/*��ʶ��*/
					flag = 0;
					mode = 0;
				}
					break;
					
			default:
					break;
		}
		
	}
}
/************���п�1�жϴ�����*************/
void UART_1Interrupt () interrupt 4
{
	uchar i;
	if(RI == 1) 
	{  //����յ�.  
		RI = 0;      //�����־.   
		Recv_buf[i] = SBUF;  //����.
		i++; 
		if(Recv_buf[i-3]==0x5a)  /*��i=3ʱ��Recv_buf[0]=0x5a,�յ���ȷ����flag��1*/
		{
			i = 0;
			flag=1;
		}
		
	}
}  

/************���п�2�жϴ�����*************/
void UART_2Interrupt() interrupt 8
{
	uchar temp2;
	if(S2CON&S2RI)
	{
		S2CON&=~S2RI;
		temp2=S2BUF;
	}
}


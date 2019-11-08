#include "stc12c5axx.h"
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int

//#define S2RI 0x01	//����2�����ж������־λ
//#define S2TI 0x02	//����2�����ж������־λ

sbit rs=P1^4;//�Ĵ���ѡ��
sbit rw=P1^5;//��д�ź���
sbit lcden=P1^6;//ledʹ�ܶ�
/*����̵���*/
sbit relay1 = P2^4;
sbit relay2 = P2^5;
sbit relay3 = P2^6;
sbit relay4 = P2^7;

/*����LED*/
sbit LED1 = P3^5;
sbit LED2 = P3^6;
sbit LED3 = P3^7;

/*DS1302�ӿ�*/
sbit rst=P3^2;//��λ��
sbit io=P3^3;//���ݿ�
sbit scl=P3^4;//ʱ����

/*�����ӿ�*/
sbit key_set_time=P2^3;//����ʱ���
sbit key_add=P2^1;//�Ӽ�
sbit key_minus=P2^2;//����
sbit key_set_alarm=P2^0;//�������Ӽ�

sbit bee=P1^1;//�������ӿ�
sbit dq=P1^0;//ds18b20����

uchar getTimebuf[7];//���ʱ������
uchar time[]={"  :  :  "};//ʱ���ʽ�ַ���
uchar date[]={"20  -  -  "};//���ڸ�ʽ�ַ���
uchar weeklist[]={"MONTUEWEDTHUFRISATSUN"};//�����ַ��б�
uchar week[]={"   "};//���ڸ�ʽ�ַ���
/*���ڽ��ܺͷ���*/
static uchar Send_buf[10] = {0} ;
static uchar Recv_buf[10] = {0} ;

int count;//�趨���ʱ�����������ʱ��count��ֵ�ֱ�Ϊ1235647
int alarm;//�Ƿ�����������ý��� 123�ֱ������ �� Сʱ������
int isOpen;//�����Ƿ���  Ĭ�ϲ�����
int fen,shi;//���ӵķ���Сʱ
int isRing;//�����Ƿ�����

uchar isInit_1302;//�Ƿ��ʼ��ʱ�����
uchar flag,k;//�������ձ�־λ
int num;
int temperature;//�¶�
int temp_flag;//�¶�������־

void UartInit(void)		//9600bps@22.1184MHz
{
	PCON &= 0x7F;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//���������ʷ�����ʱ��ΪFosc,��1T
	BRT = 0xB8;		//�趨���������ʷ�������װֵ
	AUXR |= 0x01;		//����1ѡ����������ʷ�����Ϊ�����ʷ�����
	AUXR |= 0x10;		//�������������ʷ�����
	ES = 1;
	REN = 1;
	EA = 1;
}
void delay(uint x){
	int y;
	while(x--){
		for(y=100;y>0;y--);
	}
}
void write_1602com(uchar com){
	//1602дָ��
	rs=0;
	lcden=0;
	P0=com;
	delay(5);
	lcden=1;
	delay(5);
	lcden=0;
}
void write_1602data(uchar dat){
	//1602д����
	rs=1;
	lcden=0;
	P0=dat;
	delay(5);
	lcden=1;
	delay(5);
	lcden=0;
}
void init_1602(){
	//��ʼ��1602Һ��
	rw=0;
	lcden=0;
	write_1602com(0x38);//������ʾģʽ
	write_1602com(0x0c);//��ʾ���ؼ�����Ƿ���ʾ������
	write_1602com(0x06);//����ƶ�����
	write_1602com(0x01);//����
}
void write_ds1302_byte(uchar temp){
	//ds1302дһ���ֽ�����
	uchar i;
	for(i=0;i<8;i++){
		io=temp&0x01;//�����ݷŵ�IO����
		scl=0;//sclΪ��ʱ׼������
		scl=1;//������д��
		temp>>=1;
	}
}
void write_ds1302(uchar add,uchar dat){
	//���ַaddд������dat
	rst=0;
	scl=0;
	rst=1;
	write_ds1302_byte(add);
	write_ds1302_byte(dat);
	scl=1;
	rst=0;
}
uchar read_ds1302(uchar add){
	//ds1302������
	uchar i,dat;
	rst=0;
	scl=0;
	rst=1;
	write_ds1302_byte(add);//����д��Ҫ�������ݴ��ĵ�ַ
	for(i=0;i<8;i++){
		if(io==1){
			dat|=0x80;
		}
		scl=1;
		scl=0;//�½��ض�ȡ����
		dat>>=1;
	}
	scl=1;
	rst=0;
	return dat;
}
void read_time(uchar curr_time[]){
	  uchar i;
	  uchar ucAddr = 0x81;
	  for (i=0;i<7;i++){
		curr_time[i] = read_ds1302(ucAddr);//��ʽΪ: �� �� ʱ �� �� ���� �� 
		ucAddr += 2;
	  }
}
void set_time(uchar *pSecDa){
	//�趨ʱ��
	uchar i;
	uchar ucAddr = 0x80;
	write_ds1302(0x8e,0x00);	
	for(i =7;i>0;i--){ 
		write_ds1302(ucAddr,*pSecDa); //�� �� ʱ �� �� ���� ��
		pSecDa++;
		ucAddr+=2;
	}
	write_ds1302(0x8e,0x80);
}
void init_ds1302(){
	//ds1302��ʼ��
	isInit_1302=read_ds1302(0x81);//����ʱ��״̬
	if(isInit_1302&0x80){//˵��û�г�ʼ��
		write_ds1302(0x8e,0x00);//�ر�д����  �Ժ�һֱ����
		write_ds1302(0x90,0xa5); //������Դ������� һ��������  һ��2K����
		write_ds1302(0x80,0x00);//�� CH��0 ����ʱ��
		write_ds1302(0x82,0x59);//��
		write_ds1302(0x84,0x10);//ʱ
		write_ds1302(0x86,0x07);//��
		write_ds1302(0x88,0x05);//��
		write_ds1302(0x8a,0x04);//����
		write_ds1302(0x8c,0x14);//��
		write_ds1302(0x8e,0x80);
	}
}
char int_to_char(int temp){
	//��0��9��Ӧ������תΪ�ַ�
	char x='0';
	switch(temp){
		case 0:x='0';break;
		case 1:x='1';break;
		case 2:x='2';break;
		case 3:x='3';break;
		case 4:x='4';break;
		case 5:x='5';break;
		case 6:x='6';break;
		case 7:x='7';break;
		case 8:x='8';break;
		case 9:x='9';break;
	}
	return x;
}
int ds18b20_read_temp();
void display()
{
	uchar bai,shi,ge,point,fuhao;
	read_time(getTimebuf);//ʱʱ��ȡʱ��
    time[6]=(getTimebuf[0])/16+48;//��ʽ��ʱ����
    time[7]=(getTimebuf[0])%16+48;

    time[3]=(getTimebuf[1])/16+48;//��ʽ��ʱ���
    time[4]=(getTimebuf[1])%16+48;

    time[0]=(getTimebuf[2])/16+48;//��ʽ��ʱ��Сʱ
    time[1]=(getTimebuf[2])%16+48;

    date[8]=getTimebuf[3]/16+48;//��ʽ��������
    date[9]=getTimebuf[3]%16+48;

    date[5]=getTimebuf[4]/16+48;//��ʽ��������
    date[6]=getTimebuf[4]%16+48;

    date[2]=getTimebuf[6]/16+48;//��ʽ��������
    date[3]=getTimebuf[6]%16+48;

    week[0]=weeklist[(getTimebuf[5])*3];//��ʽ������
    week[1]=weeklist[(getTimebuf[5])*3+1];
    week[2]=weeklist[(getTimebuf[5])*3+2];
	if((getTimebuf[5]%10)*3==21)//������  ���¿�ʼ
	{
		week[0]=weeklist[0];
		week[1]=weeklist[1];
		week[2]=weeklist[2];
	}
	write_1602com(0x80);
	write_1602data(' ');
	write_1602com(0x81);
	for(num=0;num<10;num++)
		{
			write_1602data(date[num]);
		}

	write_1602data(' ');
	for(num=0;num<3;num++)
		{
			write_1602data(week[num]);
		}

	write_1602com(0x80+0x40);
	for(num=0;num<8;num++)
		{
			write_1602data(time[num]);
		}
	
	//��ʾ�¶�ֵ
	write_1602com(0x80+0x40+8);//��������ָ��
	temperature=ds18b20_read_temp();
	bai=temperature/1000+0x30;
	shi=temperature%1000/100+0x30;
	ge=temperature%100/10+0x30;
	point=temperature%100%10+0x30;
	if(temp_flag==1){//˵��Ϊ����  ����ʾ����λ 125.6 25.7����
		fuhao=0x20;//��ʾ�հ�
		if(bai==0x30){
			bai=0x20;//�����λΪ0  ����ʾ
			if(shi==0x30){
				shi=0x20;//�����λΪ0  ʮλҲΪ0  ������ʾ
			}
		}
		write_1602data(fuhao);
		write_1602data(bai);
		write_1602data(shi);
	}else{
		fuhao=0x2d;//��ʾ����  -2.5  -25.8����
		write_1602data(0x20);//��Ϊ������͵�55�����Բ���ʾ��λ
		if(shi==0x30){	
			write_1602data(0x20);
			write_1602data(fuhao);
		}else{
			write_1602data(fuhao);
			write_1602data(shi);
		}
	}
	write_1602data(ge);
	write_1602data('.');
	write_1602data(point);
	write_1602data(0xdf);
	write_1602data('C');
}
void display_alarm(uchar add,int dat){
	//���趨��ʱ����ʾ����
	int x,y;
	x=dat/10;
	y=dat%10;
	write_1602com(add);
	write_1602data(int_to_char(x));
	write_1602com(add+1);//��ֹд���ַ�Զ�����һ  �����˸������
	write_1602data(int_to_char(y));
	write_1602com(add+1);
}
void init_alarm(){
	//�������ý���  ֻ���״ν����ִ��
	uchar code x[]="SET ALARM";
	uchar i;
	if(alarm==0){
		write_1602com(0x01);//����
		write_1602com(0x80+3);//��������ָ��
		for(i=0;i<9;i++){
			write_1602data(x[i]);
		}
		display_alarm(0x80+0x40+5,shi);//�������ӵ�ʱ��
		write_1602com(0x80+0x40+7);
		write_1602data(':');
		display_alarm(0x80+0x40+8,fen);
		if(isOpen){//��ʼ����ʱ������Ѿ��趨��������ʾON
			write_1602com(0x80+0x40+13);
			write_1602data(' ');
			write_1602data('O');
			write_1602data('N');
		}else{
			write_1602com(0x80+0x40+13);
			write_1602data('O');
			write_1602data('F');
			write_1602data('F');
		}
	}
}
void key_scan(){
	int i;
	uchar code tips1[]="SET SUCCESS";//�������óɹ�����ʾ
	uchar code tips2[]="CANCEL SUCCESS";//ȡ�����ӵ���ʾ
	if(key_set_time==0){//����Ƿ���
		delay(10);//����
		if(key_set_time==0){//�ٴμ���Ƿ���
			while(!key_set_time);//����Ƿ��ɿ�
			delay(10);//��ʱ����
			while(!key_set_time);//�ٴμ���Ƿ��ɿ�
			if(alarm==0){//��û����ʾ���ӽ���ʱ����ʾʱ���趨
				count++;
				write_ds1302(0x80,0x80);//��ʱ��ֹͣ
				if(count==8){
					//������ʱ��˵��ʱ���Ѿ��趨����
					write_1602com(0x0c);//�ù����ʧ
					write_ds1302(0x80,0);//��ʱ�Ӽ���
					set_time(getTimebuf);//д���µ�ʱ��
					count=0;
					return;
				}
				switch(count){
					case 1:
						write_1602com(0x80+0x40+7);//�����λ��
						break;
					case 2:
						write_1602com(0x80+0x40+4);//�ڷֵ�λ��
						break;
					case 3:
						write_1602com(0x80+0x40+1);//��ʱ��λ��
						break;
					case 4:
						write_1602com(0x80+14);//�����ڵ�λ��
						break;
					case 5:
						write_1602com(0x80+10);//���յ�λ��
						break;
					case 6:
						write_1602com(0x80+7);//���µ�λ��
						break;
					case 7:
						write_1602com(0x80+4);//�����λ��
						break;
				}
				write_1602com(0x0f);//�ù����˸
			}
		}
	}
	if(key_add==0){//����Ƿ���
		delay(10);//����
		if(key_add==0){//�ٴμ���Ƿ���
			while(!key_add);//����Ƿ��ɿ�
			delay(10);//��ʱ����
			while(!key_add);//�ٴμ���Ƿ��ɿ�
			if(count!=0){
				switch(count){
				case 1:
					//�����λ��
					getTimebuf[0]++;
					if(getTimebuf[0]==0x5a){
						getTimebuf[0]=0;
					}
					if(getTimebuf[0]==0x4a){
						getTimebuf[0]=0x50;
					}
					if(getTimebuf[0]==0x3a){
						getTimebuf[0]=0x40;
					}
					if(getTimebuf[0]==0x2a){
						getTimebuf[0]=0x30;
					}
					if(getTimebuf[0]==0x1a){
						getTimebuf[0]=0x20;
					}
					if(getTimebuf[0]==0x0a){
						getTimebuf[0]=0x10;
					}
					time[6]=(getTimebuf[0])/16+48;//��ʽ��ʱ����
					time[7]=(getTimebuf[0])%16+48;
					write_1602com(0x80+0x40+6);//�����λ��
					write_1602data(time[6]);
					write_1602com(0x80+0x40+7);//�����λ��
					write_1602data(time[7]);
					write_1602com(0x80+0x40+7);//�ù�������λ����˸
					break;
				case 2:
					//�ڷֵ�λ��
					getTimebuf[1]++;
					if(getTimebuf[1]==0x5a){
						getTimebuf[1]=0;
					}
					if(getTimebuf[1]==0x4a){
						getTimebuf[1]=0x50;
					}
					if(getTimebuf[1]==0x3a){
						getTimebuf[1]=0x40;
					}
					if(getTimebuf[1]==0x2a){
						getTimebuf[1]=0x30;
					}
					if(getTimebuf[1]==0x1a){
						getTimebuf[1]=0x20;
					}
					if(getTimebuf[1]==0x0a){
						getTimebuf[1]=0x10;
					}
					time[3]=(getTimebuf[1])/16+48;//��ʽ��ʱ���
					time[4]=(getTimebuf[1])%16+48;
					write_1602com(0x80+0x40+3);//�ڷֵ�λ��
					write_1602data(time[3]);
					write_1602com(0x80+0x40+4);//�ڷֵ�λ��
					write_1602data(time[4]);
					write_1602com(0x80+0x40+4);//�ù���ڷֵ�λ����˸
					break;
				case 3:
					//��ʱ��λ��
					getTimebuf[2]++;
					if(getTimebuf[2]==0x24){
						getTimebuf[2]=0;
					}
					if(getTimebuf[2]==0x1a){
						getTimebuf[2]=0x20;
					}
					if(getTimebuf[2]==0x0a){
						getTimebuf[2]=0x10;
					}
					time[0]=(getTimebuf[2])/16+48;//��ʽ��ʱ��Сʱ
					time[1]=(getTimebuf[2])%16+48;
					write_1602com(0x80+0x40+0);//��Сʱ��λ��
					write_1602data(time[0]);
					write_1602com(0x80+0x40+1);
					write_1602data(time[1]);
					write_1602com(0x80+0x40+1);
					break;
				case 4:
					//�����ڵ�λ��
					getTimebuf[5]++;
					if(getTimebuf[5]==0x08){
						getTimebuf[5]=0x01;
					}
					if((getTimebuf[5]%10)*3==21){//������  ���¿�ʼ
						week[0]=weeklist[0];
						week[1]=weeklist[1];
						week[2]=weeklist[2];
					}else{
						week[0]=weeklist[(getTimebuf[5]%10)*3];//��ʽ������
						week[1]=weeklist[(getTimebuf[5]%10)*3+1];
						week[2]=weeklist[(getTimebuf[5]%10)*3+2];
					}
					write_1602com(0x80+12);
					write_1602data(week[0]);
					write_1602com(0x80+13);
					write_1602data(week[1]);
					write_1602com(0x80+14);
					write_1602data(week[2]);
					write_1602com(0x80+14);
					break;
				case 5:
					//���յ�λ��
					getTimebuf[3]++;
					if(getTimebuf[3]==0x32){
						getTimebuf[3]=0x01;
					}
					if(getTimebuf[3]==0x2a){
						getTimebuf[3]=0x30;
					}
					if(getTimebuf[3]==0x1a){
						getTimebuf[3]=0x20;
					}
					if(getTimebuf[3]==0x0a){
						getTimebuf[3]=0x10;
					}
					date[8]=(getTimebuf[3])/16+48;
					date[9]=(getTimebuf[3])%16+48;
					write_1602com(0x80+9);
					write_1602data(date[8]);
					write_1602com(0x80+10);
					write_1602data(date[9]);
					write_1602com(0x80+10);
					break;
				case 6:
					//���µ�λ��
					getTimebuf[4]++;
					if(getTimebuf[4]==0x13){
						getTimebuf[4]=0x01;
					}
					if(getTimebuf[4]==0x0a){
						getTimebuf[4]=0x10;
					}
					date[5]=(getTimebuf[4])/16+48;
					date[6]=(getTimebuf[4])%16+48;
					write_1602com(0x80+6);
					write_1602data(date[5]);
					write_1602com(0x80+7);
					write_1602data(date[6]);
					write_1602com(0x80+7);
					break;
				case 7:
					//�����λ��
					getTimebuf[6]++;
					if(getTimebuf[6]==0x9a){
						getTimebuf[6]=0x00;
					}
					if(getTimebuf[6]==0x8a){
						getTimebuf[6]=0x90;
					}
					if(getTimebuf[6]==0x7a){
						getTimebuf[6]=0x80;
					}
					if(getTimebuf[6]==0x6a){
						getTimebuf[6]=0x70;
					}
					if(getTimebuf[6]==0x5a){
						getTimebuf[6]=0x60;
					}
					if(getTimebuf[6]==0x4a){
						getTimebuf[6]=0x50;
					}
					if(getTimebuf[6]==0x3a){
						getTimebuf[6]=0x40;
					}
					if(getTimebuf[6]==0x2a){
						getTimebuf[6]=0x30;
					}
					if(getTimebuf[6]==0x1a){
						getTimebuf[6]=0x20;
					}
					if(getTimebuf[6]==0x0a){
						getTimebuf[6]=0x10;
					}
					date[2]=(getTimebuf[6])/16+48;
					date[3]=(getTimebuf[6])%16+48;
					write_1602com(0x80+3);
					write_1602data(date[2]);
					write_1602com(0x80+4);
					write_1602data(date[3]);
					write_1602com(0x80+4);
					break;
				}
			}
			if(alarm!=0){
				switch(alarm){
					case 1:
						//�������ӵĿ����
						if(isOpen==0){
							isOpen=1;
							write_1602com(0x80+0x40+13);
							write_1602data(' ');
							write_1602data('O');
							write_1602data('N');
						}else{
							isOpen=0;
							write_1602com(0x80+0x40+13);
							write_1602data('O');
							write_1602data('F');
							write_1602data('F');
						}
						//��ֹд���ַ�Զ�����һ  �����˸������
						write_1602com(0x80+0x40+15);
						break;
					case 2:
						//�������ӵķ�
						fen++;
						if(fen==60){
							fen=0;
						}
						display_alarm(0x80+0x40+8,fen);
						break;
					case 3:
						//�������ӵ�Сʱ
						shi++;
						if(shi==24){
							shi=0;
						}
						display_alarm(0x80+0x40+5,shi);
						break;
				}
			}
		}
	}
	if(key_minus==0){//����Ƿ���
		delay(10);//����
		if(key_minus==0){//�ٴμ���Ƿ���
			while(!key_minus);//����Ƿ��ɿ�
			delay(10);//��ʱ����
			while(!key_minus);//�ٴμ���Ƿ��ɿ�
			if(count!=0){
				switch(count){
				case 1:
					//�����λ��
					getTimebuf[0]--;
					if(getTimebuf[0]==0xff){
						getTimebuf[0]=0x59;
					}
					if(getTimebuf[0]==0x4f){
						getTimebuf[0]=0x49;
					}
					if(getTimebuf[0]==0x3f){
						getTimebuf[0]=0x39;
					}
					if(getTimebuf[0]==0x2f){
						getTimebuf[0]=0x29;
					}
					if(getTimebuf[0]==0x1f){
						getTimebuf[0]=0x19;
					}
					if(getTimebuf[0]==0x0f){
						getTimebuf[0]=0x09;
					}
					time[6]=(getTimebuf[0])/16+48;//��ʽ��ʱ����
					time[7]=(getTimebuf[0])%16+48;
					write_1602com(0x80+0x40+6);//�����λ��
					write_1602data(time[6]);
					write_1602com(0x80+0x40+7);//�����λ��
					write_1602data(time[7]);
					write_1602com(0x80+0x40+7);//�ù�������λ����˸
					break;
				case 2:
					//�ڷֵ�λ��
					getTimebuf[1]--;
					if(getTimebuf[1]==0xff){
						getTimebuf[1]=0x59;
					}
					if(getTimebuf[1]==0x4f){
						getTimebuf[1]=0x49;
					}
					if(getTimebuf[1]==0x3f){
						getTimebuf[1]=0x39;
					}
					if(getTimebuf[1]==0x2f){
						getTimebuf[1]=0x29;
					}
					if(getTimebuf[1]==0x1f){
						getTimebuf[1]=0x19;
					}
					if(getTimebuf[1]==0x0f){
						getTimebuf[1]=0x09;
					}
					time[3]=(getTimebuf[1])/16+48;//��ʽ��ʱ���
					time[4]=(getTimebuf[1])%16+48;
					write_1602com(0x80+0x40+3);//�ڷֵ�λ��
					write_1602data(time[3]);
					write_1602com(0x80+0x40+4);//�ڷֵ�λ��
					write_1602data(time[4]);
					write_1602com(0x80+0x40+4);//�ù���ڷֵ�λ����˸
					break;
				case 3:
					//��ʱ��λ��
					getTimebuf[2]--;
					if(getTimebuf[2]==0xff){
						getTimebuf[2]=0x23;
					}
					if(getTimebuf[2]==0x1f){
						getTimebuf[2]=0x19;
					}
					if(getTimebuf[2]==0x0f){
						getTimebuf[2]=0x09;
					}
					time[0]=(getTimebuf[2])/16+48;//��ʽ��ʱ��Сʱ
					time[1]=(getTimebuf[2])%16+48;
					write_1602com(0x80+0x40+0);//��Сʱ��λ��
					write_1602data(time[0]);
					write_1602com(0x80+0x40+1);
					write_1602data(time[1]);
					write_1602com(0x80+0x40+1);
					break;
				case 4:
					//�����ڵ�λ��
					getTimebuf[5]--;
					if(getTimebuf[5]==0){
						getTimebuf[5]=0x07;
					}
					if((getTimebuf[5]%10)*3==21){//������  ���¿�ʼ
						week[0]=weeklist[0];
						week[1]=weeklist[1];
						week[2]=weeklist[2];
					}else{
						week[0]=weeklist[(getTimebuf[5]%10)*3];//��ʽ������
						week[1]=weeklist[(getTimebuf[5]%10)*3+1];
						week[2]=weeklist[(getTimebuf[5]%10)*3+2];
					}
					write_1602com(0x80+12);
					write_1602data(week[0]);
					write_1602com(0x80+13);
					write_1602data(week[1]);
					write_1602com(0x80+14);
					write_1602data(week[2]);
					write_1602com(0x80+14);
					break;
				case 5:
					//���յ�λ��
					getTimebuf[3]--;
					if(getTimebuf[3]==0){
						getTimebuf[3]=0x31;
					}
					if(getTimebuf[3]==0x2f){
						getTimebuf[3]=0x29;
					}
					if(getTimebuf[3]==0x1f){
						getTimebuf[3]=0x19;
					}
					if(getTimebuf[3]==0x0f){
						getTimebuf[3]=0x09;
					}
					date[8]=(getTimebuf[3])/16+48;
					date[9]=(getTimebuf[3])%16+48;
					write_1602com(0x80+9);
					write_1602data(date[8]);
					write_1602com(0x80+10);
					write_1602data(date[9]);
					write_1602com(0x80+10);
					break;
				case 6:
					//���µ�λ��
					getTimebuf[4]--;
					if(getTimebuf[4]==0){
						getTimebuf[4]=0x12;
					}
					if(getTimebuf[4]==0x0f){
						getTimebuf[4]=0x09;
					}
					date[5]=(getTimebuf[4])/16+48;
					date[6]=(getTimebuf[4])%16+48;
					write_1602com(0x80+6);
					write_1602data(date[5]);
					write_1602com(0x80+7);
					write_1602data(date[6]);
					write_1602com(0x80+7);
					break;
				case 7:
					//�����λ��
					getTimebuf[6]--;
					if(getTimebuf[6]==0xff){
						getTimebuf[6]=0x99;
					}
					if(getTimebuf[6]==0x8f){
						getTimebuf[6]=0x89;
					}
					if(getTimebuf[6]==0x7f){
						getTimebuf[6]=0x79;
					}
					if(getTimebuf[6]==0x6f){
						getTimebuf[6]=0x69;
					}
					if(getTimebuf[6]==0x5f){
						getTimebuf[6]=0x59;
					}
					if(getTimebuf[6]==0x4f){
						getTimebuf[6]=0x49;
					}
					if(getTimebuf[6]==0x3f){
						getTimebuf[6]=0x39;
					}
					if(getTimebuf[6]==0x2f){
						getTimebuf[6]=0x29;
					}
					if(getTimebuf[6]==0x1f){
						getTimebuf[6]=0x19;
					}
					if(getTimebuf[6]==0x0f){
						getTimebuf[6]=0x09;
					}
					date[2]=(getTimebuf[6])/16+48;
					date[3]=(getTimebuf[6])%16+48;
					write_1602com(0x80+3);
					write_1602data(date[2]);
					write_1602com(0x80+4);
					write_1602data(date[3]);
					write_1602com(0x80+4);
					break;
				}
			}
			if(alarm!=0){
				switch(alarm){
					case 1:
						//�������ӵĿ����
						if(isOpen==0){
							isOpen=1;
							write_1602com(0x80+0x40+13);
							write_1602data(' ');
							write_1602data('O');
							write_1602data('N');
						}else{
							isOpen=0;
							write_1602com(0x80+0x40+13);
							write_1602data('O');
							write_1602data('F');
							write_1602data('F');
						}
						//��ֹд���ַ�Զ�����һ  �����˸������
						write_1602com(0x80+0x40+15);
						break;
					case 2:
						//�������ӵķ�
						fen--;
						if(fen<0){
							fen=59;
						}
						display_alarm(0x80+0x40+8,fen);
						break;
					case 3:
						//�������ӵ�Сʱ
						shi--;
						if(shi<0){
							shi=23;
						}
						display_alarm(0x80+0x40+5,shi);
						break;
				}
			}
		}
	}
	if(key_set_alarm==0){//����Ƿ���
		delay(10);//����
		if(key_set_alarm==0){//�ٴμ���Ƿ���
			while(!key_set_alarm);//����Ƿ��ɿ�
			delay(10);//��ʱ����
			while(!key_set_alarm);//�ٴμ���Ƿ��ɿ�
			if(count==0){//ʱ���������߶���ʱ�������������
				init_alarm();
				alarm++;//˵�������������ý���
				if(alarm==4){
					alarm=0;//˵�������������
					write_1602com(0x01);//�����Ա���ʾʱ��
					write_1602com(0x0c);//�رչ��
					//��ʾ���óɹ���ȡ������ʾ
					if(isOpen){
						write_1602com(0x80+2);
						for(i=0;i<11;i++){
							write_1602data(tips1[i]);
						}
					}else{
						write_1602com(0x80+1);
						for(i=0;i<14;i++){
							write_1602data(tips2[i]);
						}
					}
					//��ʱ2ms��������ʾʱ��
					delay(2000);
					write_1602com(0x01);
				}else{
					switch(alarm){
						case 1:
							write_1602com(0x80+0x40+15);
							break;
						case 2:
							write_1602com(0x80+0x40+9);
							break;
						case 3:
							write_1602com(0x80+0x40+6);
							break;
					}
					write_1602com(0x0f);
				}
			}
		}
	}
}
void beep(){
	//�������  ���ұ���
	if(time[0]==int_to_char(shi/10)&&time[1]==int_to_char(shi%10)&&time[3]==int_to_char(fen/10)&&time[4]==int_to_char(fen%10)){
		isRing=1;//�������𣬴�ʱ��������������ý��� �ı�ʱ�֣����Ӿ͹ر���
		bee=0;
		delay(250);
		bee=1;
		delay(250);
	}else{
		isRing=0;//�ر����ӻ���һ���Ӻ������Զ��ر�
		bee=1;
	}
}
void delay1(int i){
	while(i--);
}
void ds18b20_init(){
	uchar x=0;
	dq = 1;    //DQ��λ
	delay1(80);  //������ʱ
	dq = 0;    //��Ƭ����DQ����
	delay1(800); //��ȷ��ʱ ���� 480us
	dq = 1;    //��������
	delay1(140);
	x=dq;      //������ʱ�� ���x=0���ʼ���ɹ� x=1���ʼ��ʧ��
	delay1(200);
}
uchar ds18b20_read(){
	//��һ���ֽ�
	uchar i=0;
	uchar dat = 0;
	for (i=8;i>0;i--)
	{
		dq = 0; // �������ź�
		dat>>=1;
		dq = 1; // �������ź�
		if(dq)
			dat|=0x80;
		delay1(40);
	}
	return(dat);
}
void ds18b20_write(char dat){
	//дһ���ֽ�
	uchar i=0;
	for (i=8; i>0; i--)
	{
		dq = 0;
		dq = dat&0x01;
		delay1(50);
		dq = 1;
		dat>>=1;
	}
}
int ds18b20_read_temp(){
	//��ȡ�¶�
	uchar low;
	uchar high;
	unsigned long tmp;
	float value;
	int t;//�¶�
	ds18b20_init();
	ds18b20_write(0xCC); //���������кŵĲ���
	ds18b20_write(0x44); //�����¶�ת��
	ds18b20_init();
	ds18b20_write(0xCC); //���������кŵĲ���
	ds18b20_write(0xBE); //��ȡ�¶ȼĴ���  ���Ÿ�  ǰ���������¶�
	low=ds18b20_read();//�Ͱ�λ����
	high=ds18b20_read();//�߰�λ����

	tmp=high;
	tmp<<=8;
	tmp=tmp|low;
	//�˴�������֮��
	if(tmp>=63488){//ffff f000 0000 0000-->(f800)
		temp_flag=0;
		//8λȫΪ1ʱ����1�Ž�λ
		if((~low)==0xff){//�ж�lowȡ����1֮���Ƿ��λ
			high=(~high)+1;
			low=0;
		}else{
			high=~high;
			low=(~low)+1;
		}
		tmp=high*256+low;
	}else{
		temp_flag=1;
	}
	value=tmp*0.0625;
	t=value*10+((temp_flag==1)?+0.5:-0.5);//�Ŵ�ʮ���������������
	return t;
}
void main()
{
	init_1602();
	init_ds1302();
	UartInit();
	LED1 = 0;
	LED2 = 0;
	LED3 = 0;
	while(1)
	{
		if(isOpen)
		{//ֻ�п������ӵ�ʱ��ż��
			beep();//���ϼ������
		}
		key_scan();
		if(count==0&&alarm==0)
		{//û���趨ʱ��  Ҳû�������ӽ����ʱ��ʱ�����ʾ
			display();
		}
		
		if(flag==1)
		{
			flag = 0;
			if((Recv_buf[2]==0x30)&&(Recv_buf[4]==0x32))
			{
				relay1 = 1;
			}
			if((Recv_buf[2]==0x31)&&(Recv_buf[4]==0x32))
			{
				relay1 = 0;
			}	
			if((Recv_buf[2]==0x30)&&(Recv_buf[4]==0x33))
			{
				relay2 = 1;
			}
			if((Recv_buf[2]==0x31)&&(Recv_buf[4]==0x33))
			{
				relay2 = 0;
			}
			if((Recv_buf[2]==0x30)&&(Recv_buf[4]==0x34))
			{
				relay3 = 1;
			}
			if((Recv_buf[2]==0x31)&&(Recv_buf[4]==0x34))
			{
				relay3 = 0;
			}
			if((Recv_buf[2]==0x30)&&(Recv_buf[4]==0x35))
			{
				relay4 = 1;
			}
			if((Recv_buf[2]==0x31)&&(Recv_buf[4]==0x35))
			{
				relay4 = 0;
			}
			if((Recv_buf[2]==0x30)&&(Recv_buf[4]==0x36))
			{
				LED1 = 0;
			}
			if((Recv_buf[2]==0x31)&&(Recv_buf[4]==0x36))
			{
				LED1 = 1;
			}			
			if((Recv_buf[2]==0x30)&&(Recv_buf[4]==0x37))
			{
				LED2 = 0;
			}
			if((Recv_buf[2]==0x31)&&(Recv_buf[4]==0x37))
			{
				LED2 = 1;
			}
			if((Recv_buf[2]==0x30)&&(Recv_buf[4]==0x38))
			{
				LED3 = 0;
			}
			if((Recv_buf[2]==0x31)&&(Recv_buf[4]==0x38))
			{
				LED3 = 1;
			}
			
		}
	}
}


/************���п�1�жϴ�����*************/
void UART_1Interrupt () interrupt 4
{
	uchar i;
	if(RI == 1) 
	{  //����յ�.  0
		
		RI = 0;      //�����־.   
		Recv_buf[i] = SBUF;  //����. 
		i++;
		if((Recv_buf[i-6]==0x53)&&(Recv_buf[i-7]==0x55))/*�ж�һ֡���ݵ���λ֡ͷ*/
		{
			i=0;
			flag=1;
		}
	}  
}


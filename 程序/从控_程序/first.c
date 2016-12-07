#include "stc12c5axx.h"
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int

//#define S2RI 0x01	//串口2接收中断请求标志位
//#define S2TI 0x02	//串口2发送中断请求标志位

sbit rs=P1^4;//寄存器选择
sbit rw=P1^5;//读写信号线
sbit lcden=P1^6;//led使能端
/*定义继电器*/
sbit relay1 = P2^4;
sbit relay2 = P2^5;
sbit relay3 = P2^6;
sbit relay4 = P2^7;

/*定义LED*/
sbit LED1 = P3^5;
sbit LED2 = P3^6;
sbit LED3 = P3^7;

/*DS1302接口*/
sbit rst=P3^2;//复位线
sbit io=P3^3;//数据口
sbit scl=P3^4;//时钟线

/*按键接口*/
sbit key_set_time=P2^3;//设置时间键
sbit key_add=P2^1;//加键
sbit key_minus=P2^2;//减键
sbit key_set_alarm=P2^0;//设置闹钟键

sbit bee=P1^1;//蜂鸣器接口
sbit dq=P1^0;//ds18b20测温

uchar getTimebuf[7];//存放时间数据
uchar time[]={"  :  :  "};//时间格式字符串
uchar date[]={"20  -  -  "};//日期格式字符串
uchar weeklist[]={"MONTUEWEDTHUFRISATSUN"};//星期字符列表
uchar week[]={"   "};//星期格式字符串
/*串口接受和发送*/
static uchar Send_buf[10] = {0} ;
static uchar Recv_buf[10] = {0} ;

int count;//设定秒分时日月星期年的时候count的值分别为1235647
int alarm;//是否进入闹钟设置界面 123分别代表开关 分 小时的设置
int isOpen;//闹钟是否开启  默认不开启
int fen,shi;//闹钟的分钟小时
int isRing;//闹钟是否在响

uchar isInit_1302;//是否初始化时钟完毕
uchar flag,k;//蓝牙接收标志位
int num;
int temperature;//温度
int temp_flag;//温度正负标志

void UartInit(void)		//9600bps@22.1184MHz
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//独立波特率发生器时钟为Fosc,即1T
	BRT = 0xB8;		//设定独立波特率发生器重装值
	AUXR |= 0x01;		//串口1选择独立波特率发生器为波特率发生器
	AUXR |= 0x10;		//启动独立波特率发生器
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
	//1602写指令
	rs=0;
	lcden=0;
	P0=com;
	delay(5);
	lcden=1;
	delay(5);
	lcden=0;
}
void write_1602data(uchar dat){
	//1602写数据
	rs=1;
	lcden=0;
	P0=dat;
	delay(5);
	lcden=1;
	delay(5);
	lcden=0;
}
void init_1602(){
	//初始化1602液晶
	rw=0;
	lcden=0;
	write_1602com(0x38);//设置显示模式
	write_1602com(0x0c);//显示开关及光标是否显示和闪动
	write_1602com(0x06);//光标移动方向
	write_1602com(0x01);//清屏
}
void write_ds1302_byte(uchar temp){
	//ds1302写一个字节数据
	uchar i;
	for(i=0;i<8;i++){
		io=temp&0x01;//将数据放到IO口上
		scl=0;//scl为低时准备数据
		scl=1;//上升沿写入
		temp>>=1;
	}
}
void write_ds1302(uchar add,uchar dat){
	//向地址add写入数据dat
	rst=0;
	scl=0;
	rst=1;
	write_ds1302_byte(add);
	write_ds1302_byte(dat);
	scl=1;
	rst=0;
}
uchar read_ds1302(uchar add){
	//ds1302读数据
	uchar i,dat;
	rst=0;
	scl=0;
	rst=1;
	write_ds1302_byte(add);//首先写入要读的数据处的地址
	for(i=0;i<8;i++){
		if(io==1){
			dat|=0x80;
		}
		scl=1;
		scl=0;//下降沿读取数据
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
		curr_time[i] = read_ds1302(ucAddr);//格式为: 秒 分 时 日 月 星期 年 
		ucAddr += 2;
	  }
}
void set_time(uchar *pSecDa){
	//设定时间
	uchar i;
	uchar ucAddr = 0x80;
	write_ds1302(0x8e,0x00);	
	for(i =7;i>0;i--){ 
		write_ds1302(ucAddr,*pSecDa); //秒 分 时 日 月 星期 年
		pSecDa++;
		ucAddr+=2;
	}
	write_ds1302(0x8e,0x80);
}
void init_ds1302(){
	//ds1302初始化
	isInit_1302=read_ds1302(0x81);//读出时钟状态
	if(isInit_1302&0x80){//说明没有初始化
		write_ds1302(0x8e,0x00);//关闭写保护  以后一直开着
		write_ds1302(0x90,0xa5); //辅助电源充电命令 一个二极管  一个2K电阻
		write_ds1302(0x80,0x00);//秒 CH置0 开启时钟
		write_ds1302(0x82,0x59);//分
		write_ds1302(0x84,0x10);//时
		write_ds1302(0x86,0x07);//日
		write_ds1302(0x88,0x05);//月
		write_ds1302(0x8a,0x04);//星期
		write_ds1302(0x8c,0x14);//年
		write_ds1302(0x8e,0x80);
	}
}
char int_to_char(int temp){
	//把0到9对应的数字转为字符
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
	read_time(getTimebuf);//时时读取时间
    time[6]=(getTimebuf[0])/16+48;//格式化时间秒
    time[7]=(getTimebuf[0])%16+48;

    time[3]=(getTimebuf[1])/16+48;//格式化时间分
    time[4]=(getTimebuf[1])%16+48;

    time[0]=(getTimebuf[2])/16+48;//格式化时间小时
    time[1]=(getTimebuf[2])%16+48;

    date[8]=getTimebuf[3]/16+48;//格式化日期日
    date[9]=getTimebuf[3]%16+48;

    date[5]=getTimebuf[4]/16+48;//格式化日期月
    date[6]=getTimebuf[4]%16+48;

    date[2]=getTimebuf[6]/16+48;//格式化日期年
    date[3]=getTimebuf[6]%16+48;

    week[0]=weeklist[(getTimebuf[5])*3];//格式化星期
    week[1]=weeklist[(getTimebuf[5])*3+1];
    week[2]=weeklist[(getTimebuf[5])*3+2];
	if((getTimebuf[5]%10)*3==21)//轮完了  重新开始
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
	
	//显示温度值
	write_1602com(0x80+0x40+8);//设置数据指针
	temperature=ds18b20_read_temp();
	bai=temperature/1000+0x30;
	shi=temperature%1000/100+0x30;
	ge=temperature%100/10+0x30;
	point=temperature%100%10+0x30;
	if(temp_flag==1){//说明为正数  不显示符号位 125.6 25.7两种
		fuhao=0x20;//显示空白
		if(bai==0x30){
			bai=0x20;//如果百位为0  不显示
			if(shi==0x30){
				shi=0x20;//如果百位为0  十位也为0  都不显示
			}
		}
		write_1602data(fuhao);
		write_1602data(bai);
		write_1602data(shi);
	}else{
		fuhao=0x2d;//显示负号  -2.5  -25.8两种
		write_1602data(0x20);//因为负数最低到55，所以不显示百位
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
	//把设定的时分显示出来
	int x,y;
	x=dat/10;
	y=dat%10;
	write_1602com(add);
	write_1602data(int_to_char(x));
	write_1602com(add+1);//防止写后地址自动向后加一  光标闪烁看不到
	write_1602data(int_to_char(y));
	write_1602com(add+1);
}
void init_alarm(){
	//闹钟设置界面  只有首次进入才执行
	uchar code x[]="SET ALARM";
	uchar i;
	if(alarm==0){
		write_1602com(0x01);//清屏
		write_1602com(0x80+3);//设置数据指针
		for(i=0;i<9;i++){
			write_1602data(x[i]);
		}
		display_alarm(0x80+0x40+5,shi);//载入闹钟的时分
		write_1602com(0x80+0x40+7);
		write_1602data(':');
		display_alarm(0x80+0x40+8,fen);
		if(isOpen){//初始化的时候如果已经设定闹钟则显示ON
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
	uchar code tips1[]="SET SUCCESS";//闹钟设置成功的提示
	uchar code tips2[]="CANCEL SUCCESS";//取消闹钟的提示
	if(key_set_time==0){//检测是否按下
		delay(10);//消抖
		if(key_set_time==0){//再次检测是否按下
			while(!key_set_time);//检测是否松开
			delay(10);//延时消抖
			while(!key_set_time);//再次检测是否松开
			if(alarm==0){//当没有显示闹钟界面时才显示时间设定
				count++;
				write_ds1302(0x80,0x80);//让时钟停止
				if(count==8){
					//继续走时，说明时间已经设定好了
					write_1602com(0x0c);//让光标消失
					write_ds1302(0x80,0);//让时钟继续
					set_time(getTimebuf);//写入新的时间
					count=0;
					return;
				}
				switch(count){
					case 1:
						write_1602com(0x80+0x40+7);//在秒的位置
						break;
					case 2:
						write_1602com(0x80+0x40+4);//在分的位置
						break;
					case 3:
						write_1602com(0x80+0x40+1);//在时的位置
						break;
					case 4:
						write_1602com(0x80+14);//在星期的位置
						break;
					case 5:
						write_1602com(0x80+10);//在日的位置
						break;
					case 6:
						write_1602com(0x80+7);//在月的位置
						break;
					case 7:
						write_1602com(0x80+4);//在年的位置
						break;
				}
				write_1602com(0x0f);//让光标闪烁
			}
		}
	}
	if(key_add==0){//检测是否按下
		delay(10);//消抖
		if(key_add==0){//再次检测是否按下
			while(!key_add);//检测是否松开
			delay(10);//延时消抖
			while(!key_add);//再次检测是否松开
			if(count!=0){
				switch(count){
				case 1:
					//在秒的位置
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
					time[6]=(getTimebuf[0])/16+48;//格式化时间秒
					time[7]=(getTimebuf[0])%16+48;
					write_1602com(0x80+0x40+6);//在秒的位置
					write_1602data(time[6]);
					write_1602com(0x80+0x40+7);//在秒的位置
					write_1602data(time[7]);
					write_1602com(0x80+0x40+7);//让光标在秒的位置闪烁
					break;
				case 2:
					//在分的位置
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
					time[3]=(getTimebuf[1])/16+48;//格式化时间分
					time[4]=(getTimebuf[1])%16+48;
					write_1602com(0x80+0x40+3);//在分的位置
					write_1602data(time[3]);
					write_1602com(0x80+0x40+4);//在分的位置
					write_1602data(time[4]);
					write_1602com(0x80+0x40+4);//让光标在分的位置闪烁
					break;
				case 3:
					//在时的位置
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
					time[0]=(getTimebuf[2])/16+48;//格式化时间小时
					time[1]=(getTimebuf[2])%16+48;
					write_1602com(0x80+0x40+0);//在小时的位置
					write_1602data(time[0]);
					write_1602com(0x80+0x40+1);
					write_1602data(time[1]);
					write_1602com(0x80+0x40+1);
					break;
				case 4:
					//在星期的位置
					getTimebuf[5]++;
					if(getTimebuf[5]==0x08){
						getTimebuf[5]=0x01;
					}
					if((getTimebuf[5]%10)*3==21){//轮完了  重新开始
						week[0]=weeklist[0];
						week[1]=weeklist[1];
						week[2]=weeklist[2];
					}else{
						week[0]=weeklist[(getTimebuf[5]%10)*3];//格式化星期
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
					//在日的位置
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
					//在月的位置
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
					//在年的位置
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
						//调节闹钟的开与关
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
						//防止写后地址自动向后加一  光标闪烁看不到
						write_1602com(0x80+0x40+15);
						break;
					case 2:
						//调节闹钟的分
						fen++;
						if(fen==60){
							fen=0;
						}
						display_alarm(0x80+0x40+8,fen);
						break;
					case 3:
						//调节闹钟的小时
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
	if(key_minus==0){//检测是否按下
		delay(10);//消抖
		if(key_minus==0){//再次检测是否按下
			while(!key_minus);//检测是否松开
			delay(10);//延时消抖
			while(!key_minus);//再次检测是否松开
			if(count!=0){
				switch(count){
				case 1:
					//在秒的位置
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
					time[6]=(getTimebuf[0])/16+48;//格式化时间秒
					time[7]=(getTimebuf[0])%16+48;
					write_1602com(0x80+0x40+6);//在秒的位置
					write_1602data(time[6]);
					write_1602com(0x80+0x40+7);//在秒的位置
					write_1602data(time[7]);
					write_1602com(0x80+0x40+7);//让光标在秒的位置闪烁
					break;
				case 2:
					//在分的位置
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
					time[3]=(getTimebuf[1])/16+48;//格式化时间分
					time[4]=(getTimebuf[1])%16+48;
					write_1602com(0x80+0x40+3);//在分的位置
					write_1602data(time[3]);
					write_1602com(0x80+0x40+4);//在分的位置
					write_1602data(time[4]);
					write_1602com(0x80+0x40+4);//让光标在分的位置闪烁
					break;
				case 3:
					//在时的位置
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
					time[0]=(getTimebuf[2])/16+48;//格式化时间小时
					time[1]=(getTimebuf[2])%16+48;
					write_1602com(0x80+0x40+0);//在小时的位置
					write_1602data(time[0]);
					write_1602com(0x80+0x40+1);
					write_1602data(time[1]);
					write_1602com(0x80+0x40+1);
					break;
				case 4:
					//在星期的位置
					getTimebuf[5]--;
					if(getTimebuf[5]==0){
						getTimebuf[5]=0x07;
					}
					if((getTimebuf[5]%10)*3==21){//轮完了  重新开始
						week[0]=weeklist[0];
						week[1]=weeklist[1];
						week[2]=weeklist[2];
					}else{
						week[0]=weeklist[(getTimebuf[5]%10)*3];//格式化星期
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
					//在日的位置
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
					//在月的位置
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
					//在年的位置
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
						//调节闹钟的开与关
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
						//防止写后地址自动向后加一  光标闪烁看不到
						write_1602com(0x80+0x40+15);
						break;
					case 2:
						//调节闹钟的分
						fen--;
						if(fen<0){
							fen=59;
						}
						display_alarm(0x80+0x40+8,fen);
						break;
					case 3:
						//调节闹钟的小时
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
	if(key_set_alarm==0){//检测是否按下
		delay(10);//消抖
		if(key_set_alarm==0){//再次检测是否按下
			while(!key_set_alarm);//检测是否松开
			delay(10);//延时消抖
			while(!key_set_alarm);//再次检测是否松开
			if(count==0){//时间在正常走动的时候才能设置闹钟
				init_alarm();
				alarm++;//说明进入闹钟设置界面
				if(alarm==4){
					alarm=0;//说明闹钟设置完毕
					write_1602com(0x01);//清屏以便显示时间
					write_1602com(0x0c);//关闭光标
					//显示设置成功或取消的提示
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
					//延时2ms后清屏显示时间
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
	//检测闹钟  并且报警
	if(time[0]==int_to_char(shi/10)&&time[1]==int_to_char(shi%10)&&time[3]==int_to_char(fen/10)&&time[4]==int_to_char(fen%10)){
		isRing=1;//闹钟响起，此时如果进入闹钟设置界面 改变时分，闹钟就关闭了
		bee=0;
		delay(250);
		bee=1;
		delay(250);
	}else{
		isRing=0;//关闭闹钟或者一分钟后闹钟自动关闭
		bee=1;
	}
}
void delay1(int i){
	while(i--);
}
void ds18b20_init(){
	uchar x=0;
	dq = 1;    //DQ复位
	delay1(80);  //稍做延时
	dq = 0;    //单片机将DQ拉低
	delay1(800); //精确延时 大于 480us
	dq = 1;    //拉高总线
	delay1(140);
	x=dq;      //稍做延时后 如果x=0则初始化成功 x=1则初始化失败
	delay1(200);
}
uchar ds18b20_read(){
	//读一个字节
	uchar i=0;
	uchar dat = 0;
	for (i=8;i>0;i--)
	{
		dq = 0; // 给脉冲信号
		dat>>=1;
		dq = 1; // 给脉冲信号
		if(dq)
			dat|=0x80;
		delay1(40);
	}
	return(dat);
}
void ds18b20_write(char dat){
	//写一个字节
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
	//读取温度
	uchar low;
	uchar high;
	unsigned long tmp;
	float value;
	int t;//温度
	ds18b20_init();
	ds18b20_write(0xCC); //跳过读序列号的操作
	ds18b20_write(0x44); //启动温度转换
	ds18b20_init();
	ds18b20_write(0xCC); //跳过读序列号的操作
	ds18b20_write(0xBE); //读取温度寄存器  共九个  前两个代表温度
	low=ds18b20_read();//低八位数据
	high=ds18b20_read();//高八位数据

	tmp=high;
	tmp<<=8;
	tmp=tmp|low;
	//此处有正负之分
	if(tmp>=63488){//ffff f000 0000 0000-->(f800)
		temp_flag=0;
		//8位全为1时，加1才进位
		if((~low)==0xff){//判断low取反加1之后是否进位
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
	t=value*10+((temp_flag==1)?+0.5:-0.5);//放大十倍输出并四舍五入
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
		{//只有开启闹钟的时候才检测
			beep();//不断检测闹钟
		}
		key_scan();
		if(count==0&&alarm==0)
		{//没有设定时间  也没有在闹钟界面的时候时间才显示
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


/************串行口1中断处理函数*************/
void UART_1Interrupt () interrupt 4
{
	uchar i;
	if(RI == 1) 
	{  //如果收到.  0
		
		RI = 0;      //清除标志.   
		Recv_buf[i] = SBUF;  //接收. 
		i++;
		if((Recv_buf[i-6]==0x53)&&(Recv_buf[i-7]==0x55))/*判断一帧数据的两位帧头*/
		{
			i=0;
			flag=1;
		}
	}  
}


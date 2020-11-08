#include"uart.h"
#include<reg52.h>

void Uart_Init(void)
{
  PCON |= 0x80;		//使能波特率倍速位SMOD
	SCON = 0x50;		//8位数据,可变波特率
//	AUXR &= 0xBF;		//定时器1时钟为Fosc/12,即12T
//	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
//	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xFF;		//设定定时初值
	TH1 = 0xFF;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
  EA=1;
}


void Uart_Send_Byte(unsigned char c)//UART Send a byte
{
	SBUF = c;
	while(!TI);		//发送完为1 
	TI = 0;
}

unsigned char Uart_Receive_Byte()//UART Receive a byteg
{	
	unsigned char dat;
	while(!RI);	 //接收完为1 
	RI = 0;
	dat = SBUF;
	return (dat);
}

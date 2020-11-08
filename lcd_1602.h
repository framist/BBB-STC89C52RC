#ifndef __LCD__
#define __LCD__

#include "reg52.h"
extern char local_date,base_date;
extern unsigned char code *Main_Menu[];



//LCD1602����λ����
#define 	DataPort  	P0
sbit 		RS 			= P2^5;     //�Ĵ���ѡ�� 0:ָ��Ĵ��� 1:���ݼĴ���
sbit 		RW 			= P2^6;     //��д���� 0��д  1����
sbit 		EN 			= P2^7;     //��д����ʹ��   0��ֹͣ 1������

//1602����ַ����
#define LINE1 0x80				   //��һ�е�ַ
#define LINE2 0xc0				   //�ڶ��е�ַ

//�ⲿ����
//void Delay_us(unsigned int us);
//void Delay_ms(unsigned int ms);
void LCD1602_WriteCMD(unsigned char cmd);
void LCD1602_WriteDAT(unsigned char dat);
void LCD1602_CheckBusy(void);
void LCD1602_Init(void);
void LCD1602_Display(unsigned char addr,unsigned char* pointer,unsigned char index,unsigned char num);



#endif

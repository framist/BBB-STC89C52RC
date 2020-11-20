
#include "eeprom.h"

 

/**********��ISP/IAP����**************/   

void ISP_IAP_Enable(void)

{

    EA = 0;  //���ж�  

    ISP_CONTR = ISP_CONTR & 0x18;  

    ISP_CONTR = ISP_CONTR | WaitTime; // ���õȴ�ʱ��

    ISP_CONTR = ISP_CONTR | 0x80;   //����ISP/IAP����

}

 

/**********�ر�ISP/IAP����**************/   

void ISP_IAP_Disable(void)

{

	 ISP_CONTR = ISP_CONTR & 0x7f; //��ֹISP/IAP����

     ISP_CMD = 0x00;    //ȥ��ISP/IAP����

	 ISP_TRIG = 0x00;   //��ֹISP/IAP�����޴���

	 EA   =   1;        // ���ж� 

}

 

/**********����ISP/IAP**************/  

void ISPTrig(void)

{

	 ISP_TRIG = 0x46;  //����46h������B9h��ISP/IAP�����Ĵ�����ÿ�ζ������

	 ISP_TRIG = 0xb9;  //����B9h��ISP/IAP������������������

	 _nop_();

}

 

/**********�ֽڶ�**************/

unsigned char byte_read(unsigned int byte_addr)

{

     unsigned char  dat = 0;    //��EEPROM���ݻ���

 

	 EA = 0;     //���ж�

	 ISP_ADDRH = (unsigned char)(byte_addr >> 8);     //�͵�ַ���ֽ�

	 ISP_ADDRL = (unsigned char)(byte_addr & 0x00ff); //�͵�ַ���ֽ�

     ISP_IAP_Enable();             //��ISP/IAP����

	 ISP_CMD   = ISP_CMD & 0xf8;   //���ISP_CMD�Ĵ�����3λ

	 ISP_CMD   = ISP_CMD | RdCommand; //д����������� 

	 ISPTrig();                //����ISP/IAP

	 dat = ISP_DATA;           //��ISP_DATA�Ĵ����е����ݱ�������

     ISP_IAP_Disable();        //�ر�ISP/IAP���� 

	 EA  = 1;           //ʹ���ж�

	 return dat;        //���ض���������

}

 

/**********�ֽ�д**************/

void byte_write(unsigned int byte_addr,unsigned char Orig_data)

{

	 EA  = 0;  //���ж�

	 ISP_ADDRH = (unsigned char)(byte_addr >> 8);       //�͵�ַ���ֽ�

	 ISP_ADDRL = (unsigned char)(byte_addr & 0x00ff);   //�͵�ַ���ֽ�

	 ISP_IAP_Enable();                   //��ISP/IAP����

     ISP_CMD  = ISP_CMD & 0xf8;         //���ISP_CMD�Ĵ�����3λ

	 ISP_CMD  = ISP_CMD | PrgCommand;   //д��д�������� 

	 ISP_DATA = Orig_data;      //д�����ݵ�ISP_DATA�Ĵ���

	 ISPTrig();                //����ISP/IAP

	 ISP_IAP_Disable();        //�ر�ISP/IAP���� 

	 EA =1;            //ʹ���ж�

}

/**********��������**************/

void SectorErase(unsigned int sector_addr)

{

	 EA = 0;   

	 ISP_ADDRH = (unsigned char)(sector_addr >> 8);        //�Ͳ�����ַ���ֽ�

	 ISP_ADDRL = (unsigned char)(sector_addr & 0x00ff);    //�Ͳ�����ַ���ֽ�

	 ISP_IAP_Enable(); 

     ISP_CMD = ISP_CMD & 0xf8;           //���ISP_CMD�Ĵ�����3λ

	 ISP_CMD = ISP_CMD | EraseCommand;   //д�������������

	 ISPTrig();       //����ISP/IAP

	 ISP_IAP_Disable();    //�ر�ISP/IAP���� 

 

}

#include "buzz.h"
#include "delay.h"

void Buzz_Times(unsigned char times)
{
	unsigned char i=0;
	for(i=0;i<times;i++)
	{
		 buzz=0;
		 Delay_Ms(200);
		 buzz=1;
		 Delay_Ms(200);
	}
}

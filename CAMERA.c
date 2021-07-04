#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#define LED1 5 // wPi 5, BCM_GPIO 24
#define TC1 4 //touchsensor

char cmd[] ="sudo raspistill -o /home/pi/Downloads/Pictures/test.jpg";

int main(void)
{
	if(wiringPiSetup()==-1) return 1; 

	// TC1 on-off ...
	pinMode(TC1, INPUT);
	
	while(1)
	{
		if(digitalRead(TC1)==0)
		{
			printf("Not Detect !!\n");
		}
		else
		{
			printf("Detected !!\n");
			system(cmd);
		}
		delay(1000);
	}
}

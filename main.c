#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include "iotmakers.h"
#include <softPwm.h>

#define Servo 1
#define Trig 0
#define Echo 2


static int local_loop = (0);

static void SigHandler(int sig)
{
	switch(sig)
	{
		case SIGTERM :
		case SIGINT :
			printf("accept signal SIGINT[%d]\n", sig);
			im_stop_service();
			local_loop = (0);
			break;
		default :
			;
	};
	return;
}
static void set_SigHandler()
{
	signal(SIGINT,   SigHandler);
	signal(SIGTERM,  SigHandler);
}

/* ============================
main_sample1.c

- Receiving the control data
	mycb_numdata_handler();
	mycb_strdata_handler();
============================ */
static void mycb_numdata_handler(char *tagid, double numval)//iot makers
{
	// !!! USER CODE HERE
	printf("tagid=[%s], val=[%f]\n", tagid, numval);

	softPwmCreate(Servo, 0, 200);
	if(!strcmp(tagid,"Servo"))
	{
		if(numval == 1)
		{
			delay(3000);
			softPwmWrite(Servo, 5);
			delay(3000);//0 degree
		}
		if(numval == 0)
		{
			delay(3000);
			softPwmWrite(Servo, 24);
			delay(3000);//0 degree
		}
	}

}
/* ============================
main_sample1.c

- Sending the collection data
	im_send_numdata();
	im_send_strdata();
============================ */
int main()
{
	int i;
	int rc;
	float distance, start, stop;
	
	if(wiringPiSetup() == -1) exit(1);
	
	pinMode(Trig, OUTPUT);
	pinMode(Echo, INPUT);
	softPwmCreate(1, 0, 200);
	
	set_SigHandler();
	
	printf("im_init()\n");
	
	rc = im_init_with_config_file("./config.txt");
	
	if(rc < 0)
	{
		printf("fail im_init()\n");
		return -1;
	}

	im_set_loglevel(LOG_LEVEL_DEBUG);
	im_set_numdata_handler(mycb_numdata_handler);

	printf("im_start_service()...\n");
	
	rc = im_start_service();
	
	if(rc < 0)
	{
		printf("fail im_start_service()\n");
		im_release();
		return -1;
	}

	// Raspberry pi wiringPiSetup...
	printf("start\n");
	
	local_loop = (1);
	
	while(local_loop == (1))
	{

	// detect change and read data...
	digitalWrite(Trig,LOW);
	delay(3000);
	digitalWrite(Trig,HIGH);
	delayMicroseconds(10);
	digitalWrite(Trig,LOW);
	while (digitalRead(Echo)==0);
	start = micros();
	while(digitalRead(Echo)==1);
	stop=micros();
	distance = (stop - start)/58;
	im_send_numdata("Ultrasonic",distance,0);
	/*when you want to control servomotor without iotmakers use below
	if (distance >=50){
		softPwmWrite(Servo, 5);//0 degree
	}
	else{
		softPwmWrite(Servo, 24);//90 degree
		}
	printf("distance %.2f cm\n",distance);


	delay(1000);*/
	}

	printf("im_stop_service()\n");
	im_stop_service();

	printf("im_release()\n");
	im_release();

	return 0;
}


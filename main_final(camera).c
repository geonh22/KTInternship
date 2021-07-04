#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include "iotmakers.h"
#include <softPwm.h>

char cmd[] ="sudo raspistill -o /home/pi/Downloads/Pictures/test.jpg";

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

	if(!strcmp(tagid,"Camera"))
	{
		if(numval == 1)
		{
			system(cmd);
			printf("picturesaved");
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
	
	if(wiringPiSetup() == -1) exit(1);
	
	
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
	{}

	printf("im_stop_service()\n");
	im_stop_service();

	printf("im_release()\n");
	im_release();

	return 0;
}


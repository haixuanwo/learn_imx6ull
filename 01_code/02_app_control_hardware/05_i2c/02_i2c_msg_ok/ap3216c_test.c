
#include <stdio.h>
#include <unistd.h>
#include "ap3216c_lib.h"

int main(int argc, char **argv)
{
	int err; 
	int light, distance;
	int cnt = 0;
	
	err = ap3216c_init();
	if (err)
	{
		printf("ap3216c_init err : %d\n", err);
	}

	while (1)
	{
		light = ap3216c_read_light();	
		distance = ap3216c_read_distance();
		printf("%03d: light = %d, distance = %d\n", cnt, light, distance);
		cnt++;
		sleep(5);
	}


	return 0;
}


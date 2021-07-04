/*
 ============================================================================
 Name        : .c
 Author      : Hda
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>


#define LED_PATH				"/sys/class/leds/beaglebone:green:usr%d/trigger"
#define Led_Number			3
#define Size					100
#define insertThreeArguments	3


int processTriggerValues (char argv[]);
void controlBrightness(int value);


int processTriggerValues (char argv[])
{
	if ( ! ( strcmp(argv, "timer") && strcmp(argv, "default-on")
			&& strcmp(argv, "none") && strcmp(argv, "oneshot") && strcmp(argv, "heartbeat") ) )
	{
		FILE *fd;
		char buff [Size];

		snprintf(buff, sizeof (buff), LED_PATH,Led_Number);

		fd = fopen(buff, "a+");

		if (fd == NULL)
		{
			perror("Write Trigger Error\n");
			exit(0);
		}
		fwrite((void*)argv, strlen(argv), sizeof(argv), fd);

		fclose(fd);

		printf("Writing Successful\n");

	}
}


int main(int argc, char *argv[])
{
	printf("**** This application controls the USER Led3 *******\n");

#if 1
	switch (argc)
	{
	case insertThreeArguments:
	{
		if (strcmp (argv[1], "trigger") == 0)
		{
			processTriggerValues (argv[2]);
		}
		else if (strcmp (argv[1], "brightness") == 0)
		{
//			int value = atoi(argv[2]);
//			controlBrightness(value);
		}
		else
		{
			printf("Invalid control option\n");
			printf( "Valid Control Options : Brightness, Trigger\n");
		}
		break;
	}
	default:
	{
		printf( "usage: %s < Control Options> <Value>\n", argv[0] );
		printf( "valid control_options : brightness, trigger\n");
		printf ("valid 'brightness' values : 0,1\n");
		printf("valid 'trigger' values : heartbeat,timer,none,oneshot,default-on\n");
		break;
	}
	}

#endif

}


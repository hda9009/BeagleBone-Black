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
#define Size					100
#define fourArgumentInserted	4

int ledNumber;

int processTriggerValues (int led, char argv[]);
//void controlBrightness(int value);


int processTriggerValues (int led, char argv[] )
{
	if ( ! ( strcmp(argv, "timer") && strcmp(argv, "default-on")
			&& strcmp(argv, "none") && strcmp(argv, "oneshot") && strcmp(argv, "heartbeat") ) )
	{
		FILE *fd;
		char buff [Size];

		snprintf(buff, sizeof (buff), LED_PATH, led);

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
	else
	{
		printf("Invalid Entry\n");
	}
	return 0;
}


int main(int argc, char *argv[])
{
	printf("**** This application controls the USER Led: %d *******\n", argc - 1);

#if 1
	switch (argc)
	{
	case fourArgumentInserted:
	{
		if (atoi(argv[1]) >= 0  && atoi(argv[1]) <= 3)
		{
			ledNumber = atoi(argv[1]);
			if (strcmp (argv[2], "trigger") == 0)
			{
				processTriggerValues (ledNumber, argv[3]);
			}
			else if (strcmp (argv[2], "brightness") == 0)
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

		else
		{
			printf("User Leds Range [0, 3]");
		}

		break;
	}
	default:
	{
		printf ("Usage: %s: < User Led Number>  < trigger >  < Trigger Options > ", argv[0]);
		printf("Trigger Options : heartbeat, timer, none, oneshot, default-on\n");
		break;
	}
	}

#endif

}




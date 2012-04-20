/*
  Simple C++ program to toggle BeagleBone leds
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>


#define NUM_LEDS 4

char ledSysPath[NUM_LEDS][64] = {
	"/sys/class/leds/beaglebone::usr0",
	"/sys/class/leds/beaglebone::usr1",
	"/sys/class/leds/beaglebone::usr2",
	"/sys/class/leds/beaglebone::usr3"
};

bool configureLeds();
bool restoreLeds();
bool setLeds(int mask);	
void setSigHandler();

bool shutdown = false;

void sigHandler(int sig)
{
	if (sig == SIGINT)
		shutdown = true;
}

int main(int argc, char **argv)
{
	int mask;

	setSigHandler();

	if (!configureLeds())
		exit(1);

	mask = 1;

	while (!shutdown) {
		//if (!setLeds(mask))
		//	exit(1);

		sleep(1);	

		mask <<= 1;

		if (mask & 0x10)
			mask = 1;
	}	

	restoreLeds();

	return 0;	
}

bool setLeds(int mask)
{
	int fd;
	char name[128];

	for (int i = 0; i < NUM_LEDS; i++) {
		strcpy(name, ledSysPath[i]);
		strcat(name, "/brightness");

		fd = open(name, O_RDWR);
		if (fd < 0) {
			perror("open brightness");
			return false;
		}

		if (write(fd, (mask & 0x01) ? "1" : "0", 1) != 1) {
			perror("write brightness");
			close(fd);
			return false;
		}

		close(fd);

		mask >>= 1;
	}	

	return true;
}

// Set the trigger mode of the leds to 'gpio'
// Enable gpio and set the brightness to zero
bool configureLeds()
{
	int fd;
	char name[128];

	for (int i = 0; i < NUM_LEDS; i++) {
		strcpy(name, ledSysPath[i]);
		strcat(name, "/trigger");

		fd = open(name, O_RDWR);
		if (fd < 0) {
			perror("open trigger");
			return false;
		}

		if (write(fd, "gpio", 4) != 4) {
			perror("write trigger = gpio");
			close(fd);
			return false;
		}	

		close(fd);

		strcpy(name, ledSysPath[i]);
		strcat(name, "/gpio");

		fd = open(name, O_RDWR);
		if (fd < 0) {
			perror("open gpio");
			return false;
		}

		if (write(fd, "1", 1) != 1) {
			perror("write gpio = 1");
			close(fd);
			return false;
		}

		close(fd);

		strcpy(name, ledSysPath[i]);
		strcat(name, "/brightness");

		fd = open(name, O_RDWR);
		if (fd < 0) {
			perror("open brightness");
			return false;
		}
	
		if (write(fd, "0", 1) != 1) {
			perror("write brightness = 0");
			close(fd);
			return false;
		}

		close(fd);	
	}	

	return true;
}

bool restoreLeds()
{
	int fd;
	char name[128];
	char val[16];

	setLeds(0);

	for (int i = 0; i < NUM_LEDS; i++) {
		strcpy(name, ledSysPath[i]);
		strcat(name, "/trigger");

		fd = open(name, O_RDWR);
		if (fd < 0) {
			perror("open trigger");
			return false;
		}

		if (i == 0)
			strcpy(val, "heartbeat");
		else if (i == 1)
			strcpy(val, "mmc0");
		else
			strcpy(val, "none");

		if (write(fd, val, strlen(val)) != (int)strlen(val)) {
			perror("write trigger = default");
			close(fd);
			return false;
		}	

		close(fd);
	}

	return true;
}

void setSigHandler()
{
	struct sigaction sia;

	bzero(&sia, sizeof(sia));
	sia.sa_handler = sigHandler;

	if (sigaction(SIGINT, &sia, NULL) < 0) {
		perror("sigaction(SIGINT)");
		exit(1);
	}
}

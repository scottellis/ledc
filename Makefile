#
# Makefile for ledc demo for toggling BeagleBone LEDs
#

CC = g++
CFLAGS = -O2 -Wall

ledc : ledc.c
	$(CC) $(CFLAGS) ledc.c -o ledc

clean:
	rm -f ledc


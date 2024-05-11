/*
 * serialTest.c:
 *	Very simple program to test the serial port. Expects
 *	the port to be looped back to itself
 *
 * Copyright (c) 2012-2013 Gordon Henderson. <projects@drogon.net>
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <pthread.h>
#include <unistd.h>

int fd;

void* sendHandler(){

	char *sendBuf;
	sendBuf = (char *)malloc(32*sizeof(char));

	while(1){
		memset(sendBuf,'\0',32);
		scanf("%s",sendBuf);
		while(*sendBuf != '\0'){
			serialPutchar(fd,*sendBuf++);
		}
	}

}

int main ()
{
  int count ;
  unsigned int nextTime ;

  pthread_t idSend;

  if ((fd = serialOpen ("/dev/ttyS5", 115200)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  pthread_create(&idSend,NULL,sendHandler,NULL);

  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }

 // pthread_create(&idSend,NULL,sendHandler,NULL);

  while(1){
    while (serialDataAvail (fd))
    {
      printf ("%c", serialGetchar(fd)) ;
      fflush (stdout) ;
    }
	usleep(10000);

	//printf ("\n") ;
  }

  return 0 ;
}

#include <stdio.h>
#include <sys/time.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <unistd.h>

#define Trig 0
#define Echo 1

double getDistance(){

	long difftime;
	double dis;

	struct timeval t_start;
	struct timeval t_stop;

	pinMode(Trig,OUTPUT);
	pinMode(Echo,INPUT);

	digitalWrite(Trig,LOW);
	usleep(5);
	digitalWrite(Trig,HIGH);
	usleep(10);
	digitalWrite(Trig,LOW);

	while(!digitalRead(Echo));
	gettimeofday(&t_start,NULL);
	while(digitalRead(Echo));
	gettimeofday(&t_stop,NULL);

	difftime = 1000000 * (t_stop.tv_sec - t_start.tv_sec) + (t_stop.tv_usec - t_start.tv_usec);
	
	dis = (double) difftime / 1000000 * 34000 / 2;

	return dis;
}

int main(){

	if(wiringPiSetup() == -1){
		
		fprintf(stderr,"%s","wiringPi error");
		exit(-1);
	}	

	double dis;

	while(1){
		
		dis = getDistance();

		printf("the object distance is %lfcm\n",dis);
		usleep(500000);

	}


	return 0;
}

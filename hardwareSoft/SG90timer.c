#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <wiringPi.h>

#define SG90 5

//搞清楚静态变量的用法，什么时候用最合适。
static int i = 0;
int jd;

void signal_handler(int signum){

	if(i<jd){
		digitalWrite(SG90,HIGH);
	}else{
		digitalWrite(SG90,LOW);
	}
	if(i == 40){
		i = 0;
	}	
	
	i++;
}

int main(){

	struct itimerval itv;

	wiringPiSetup();
	pinMode(SG90,OUTPUT);

	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 500;

	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;

	if(setitimer(ITIMER_REAL,&itv,NULL) == -1){
	
		perror("error");
		exit(-1);
	};

	
	signal(SIGALRM,signal_handler);

	while(1){
		printf("input:1-0、2-45、3-90、4-135、5-180");
		scanf("%d",&jd);
	};

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>

static int i;

void signal_handler(int signum){

	i++;
	for(i=0;i<2000;i++){
		printf("hello\n");
		i = 0;
	}	
	

}

int main(){

	struct itimerval itv;

	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 500;

	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;

	if(setitimer(ITIMER_REAL,&itv,NULL) == -1){
	
		perror("error");
		exit(-1);
	};

	
	signal(SIGALRM,signal_handler);

	while(1);

	return 0;
}

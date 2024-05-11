/*int gettimeofday (struct timeval * tv, struct timezone * tz);
 * timeval 定义：

struct timeval{
    long tv_sec;  //秒
    long tv_usec;  //微秒
};

*/

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

void cntTest(){
	int i;
	int j;
	for(i=0;i<100;i++){
		for(j=0;j<100;j++){
		
		}
	};
}


int main(){

	struct timeval t_start;
	struct timeval t_stop;

	gettimeofday(&t_start,NULL);

	cntTest();

	gettimeofday(&t_stop,NULL);

	int difftime = 1000 * (t_stop.tv_sec - t_start.tv_sec) + (t_stop.tv_usec - t_start.tv_usec);
	
	printf("the time difference is %d \n",difftime);
		
	return 0;
}

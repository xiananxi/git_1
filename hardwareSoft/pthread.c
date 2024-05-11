#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void* sendHandler(void *arg){
	//static的原因：线程退出后，ret的值不会被销毁
	static int ret = 10;
	printf("sendHandler %ld thread is create\n",(unsigned long)pthread_self());
	printf("sendHandler 的pid是 %d\n",getpid());
	printf("sendHandler param is %d\n",*(int *)arg);
	
	pthread_exit((void *)&ret);
	return (void *)0;
}

int main(){
	
	//int pthread_create(pthread_t *restrict tidp, const pthread_attr_t *restrict attr, void *(*start_rtn)(void *), void *restrict arg);
	
	int ret;
	int param = 100;
	int *pret;

	pthread_t idSend;
	
	ret = pthread_create(&idSend,NULL,sendHandler,(void *)&param);
	
//	sleep(1);

	if(ret == 0){
	
		printf("create idSend success\n");
		printf("main 的pid%d\n",getpid());
		printf("main id is %ld \n",(unsigned long)pthread_self());
	
	}

	pthread_join(idSend,(void **)&pret);

	printf("main idSend quit:%d\n",*pret);

	while(1){
		sleep(10);
	};

	return 0;
}

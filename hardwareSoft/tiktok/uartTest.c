#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "wiringSerial.h"
#include <pthread.h>
#include "uartTool.h"

int fd;
void* readSerial(){
	
//	char buffer[32];
	char cmd;
	while(1){
	//	memset(cmd,'\0',sizeof(char));
//		serialGetstring(fd,buffer);
		cmd = myserialGetchar(fd);
		//printf("GET ->0x%c\n",cmd);
		switch(cmd){
			case 'N':
				printf("next\n");
				system("adb shell input swipe 540 1300 540 500 100");
				break;

			case 'P':
				printf("pre\n");
				system("adb shell input swipe 540 500 540 1300 100");
				break;

			case 'Z':
				printf("zan\n");
				system("adb shell \"seq 3 | while read i;do input tap 350 1050 & input tap 350 1050 &sleep 0.01;done;\"");
				break;
			
			case 'Q':
				printf("close\n");
				system("adb shell input keyevent 26");
				break;
		}
	}
}

int main(int argc,char **argv){

	char deviceName[32] = {'\0'};
	pthread_t readt;

	if(argc < 2){
		printf("uage:%s /dev/ttys?\n",argv[0]);
		exit(-1);
	}
	strcpy(deviceName,argv[1]);
	if((fd = myserialOpen(deviceName,115200) ) == -1){
		printf("open %s error\n",deviceName);
		return -1;
	}
	
	pthread_create(&readt,NULL,readSerial,NULL);

	while(1){sleep(10);}

}

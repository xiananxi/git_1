#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "uartTool1.h"
#include <unistd.h>
#include <pthread.h>
 
int fd;
 
void *Sendhandler(){
    char sendBuf[32];
    while(1){
        memset(sendBuf,'\0',32);
        scanf("%s",sendBuf);
        serialSendstring (fd,sendBuf);
    }
}
 
void *Readhandler(){
    char readBuf[32];
    while(1){
        memset(readBuf,'\0',32);
        serialGetstring(fd,readBuf);
        printf("GET ->%s\n",readBuf);
    }
}
 
int main (int argc,char **argv)
{
    char deviceName[32] = {'\0'};
 
    strcpy(deviceName,argv[1]);
 
//A.  打开串口并配置波特率为115200
    fd = myserialOpen (deviceName, 115200);
 
//C.  创建线程，用于发送数据
    pthread_t idSend;
    pthread_t idRead;
    pthread_create(&idSend,NULL,Sendhandler,NULL);
    pthread_create(&idRead,NULL,Readhandler,NULL);
//D.  循环检测数据，接收数据
    
    while(1){
        sleep(30);
    }
  return 0 ;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPi.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h> // 对于AF_INET套接字地址族
#include <netinet/tcp.h> // 这个头文件定义了SOL_TCP

#include "uartTool.h"
#include "garbage.h"
#include "pwm.h"
#include "myoled.h"
#include "socket.h"

int serial_fd = -1;
pthread_cond_t cond;
pthread_mutex_t mutex;

void *pget_voice(void *arg){
    unsigned char buffer[6] = {0xAA,0x55,0x00,0x00,0x55,0xAA}; 
        int len = 0;
        printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
        if(-1 == serial_fd)
        {
            printf("%s|%s|%d: open serial failed\n", __FILE__, __func__, __LINE__);
            pthread_exit(0);
        }
        printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    while(1){      
        len = serialGetstring(serial_fd,buffer);
        printf("%s|%s|%d, len=%d\n", __FILE__, __func__, __LINE__,len);
        if(len > 0 && buffer[2] == 0x46){
            pthread_mutex_lock(&mutex);
            buffer[2] = 0x00;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_exit(0);
}

void *psend_voice(void *arg){
    pthread_detach(pthread_self());//????
    unsigned char *buffer = (unsigned char *)arg;

    if (-1 == serial_fd){
        printf("%s|%s|%d: open serial failed\n", __FILE__, __func__, __LINE__);
        pthread_exit(0);
    }
    if(NULL != buffer){
        serialSendstring(serial_fd,buffer,6);
    }
    pthread_exit(0);
}

void *popen_trash_can(void *arg){
    pthread_detach(pthread_self());
    //子线程调用pthread_detach(pthread_self())就是分离自己，
    //因为pthread_self()这个函数返回的就是自己本身的线程ID；
    
    unsigned char *buffer = (unsigned char *)arg;
    if (buffer[2] == 0x43)
    {
        printf("%s|%s|%d: buffer[2]=0x%x\n", __FILE__, __func__, __LINE__,buffer[2]);
        pwm_write(PWM_RECOVERABLE_GARBAGE);
        delay(2000);
        pwm_stop(PWM_RECOVERABLE_GARBAGE);
    }
    else if(buffer[2] != 0x45)
    {
        printf("%s|%s|%d: buffer[2]=0x%x\n", __FILE__, __func__, __LINE__,
        buffer[2]);
        pwm_write(PWM_GARBAGE);
        delay(2000);
        pwm_stop(PWM_GARBAGE);
    }
    pthread_exit(0);
}

void *poled_show(void *arg)
{
    pthread_detach(pthread_self());
    myoled_init();
    oled_show(arg);
    pthread_exit(0);
}

void *pcategory(void *arg){
    unsigned char buffer[6] = {0xAA, 0x55, 0x00, 0x00, 0X55, 0xAA};
    char *category = NULL;
    pthread_t send_voice_tid, trash_tid,oled_tid;
    while(1){
        printf("%s|%s|%d: \n", __FILE__, __func__, __LINE__);
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);//接下来的线程拿到锁之后
        pthread_mutex_unlock(&mutex);
        printf("%s|%s|%d: \n", __FILE__, __func__, __LINE__);

        buffer[2] = 0x00;
        system(WGET_CMD);

        if(0 == access(GARBAGE_FILE,F_OK)){
            //照片存在的话，阿里云识别之后会返回识别结果
                    category = garbage_category(category);
                    if(strstr(category,"干垃圾")){
                        buffer[2] = 0x41;
                    }else if(strstr(category,"湿垃圾")){
                        buffer[2] = 0x42;
                    }else if(strstr(category,"可回收垃圾")){
                        buffer[2] = 0x43;
                    }else if(strstr(category,"有害垃圾")){
                        buffer[2] = 0x44;
                    }else{
                        buffer[2] = 0x45;
                    }                                   
        }else{
                buffer[2] = 0x45;
             }
        pthread_create(&send_voice_tid,NULL,psend_voice,(void *)buffer);
        pthread_create(&trash_tid,NULL,popen_trash_can,(void *)buffer);
        pthread_create(&oled_tid, NULL, poled_show, (void *)buffer);

        remove(GARBAGE_FILE);
        //不需要发送数据给buffer了嘛？因为这里使用了线程？对
        //serialSendstring(serial_fd,buffer,6);
    }
    pthread_exit(0);
}

void *pget_socket(void *arg){

    int s_fd = -1;
    int c_fd = -1;
    char buffer[6];
    int nread = -1;
    
    struct sockaddr_in c_addr;
    memset(&c_addr,0,sizeof(struct sockaddr_in));

    s_fd = socket_init(IPADDR,IPPORT);
    printf("%s|%s|%d:s_fd=%d\n", __FILE__, __func__, __LINE__, s_fd);
    if(-1 == s_fd){
        pthread_exit(0);
    }
    sleep(3);

    int clen = sizeof(struct sockaddr_in);

    while(1){
        c_fd = accept(s_fd,(struct sockaddr *)&c_addr,&clen);

        int keepalive = 1; // 开启TCP KeepAlive功能
        int keepidle = 5; // tcp_keepalive_time 3s内没收到数据开始发送心跳包
        int keepcnt = 3; // tcp_keepalive_probes 每次发送心跳包的时间间隔,单位秒
        int keepintvl = 3; // tcp_keepalive_intvl 每3s发送一次心跳包

        setsockopt(c_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive,sizeof(keepalive));
        setsockopt(c_fd, SOL_TCP, TCP_KEEPIDLE, (void *) &keepidle, sizeof(keepidle));
        setsockopt(c_fd, SOL_TCP, TCP_KEEPCNT, (void *)&keepcnt, sizeof(keepcnt));
        setsockopt(c_fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepintvl, sizeof(keepintvl));
        printf("%s|%s|%d: Accept a connection from %s:%d\n",__FILE__,__func__,__LINE__, inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
    
        if(c_fd == -1){
            perror("accept");
            continue;//?
        }

        while(1){
            memset(buffer,0,sizeof(buffer));
            nread = recv(c_fd,buffer,sizeof(buffer),0);
            printf("%s|%s|%d:nread=%d,buffer=%s\n",__FILE__,__func__,__LINE__,nread,buffer);
            if(nread > 0){
                if(strstr(buffer,"open")){
                    pthread_mutex_lock(&mutex);
                    pthread_cond_signal(&cond);
                    pthread_mutex_unlock(&mutex);
                }
            }else if(nread == 0 || nread == -1){
                break;
            }
        }
        close(c_fd);
    }
    pthread_exit(0);
}

static int detect_process(const char * process_name) //判断进程是否在运行
{
    int n = -1;
    FILE *strm;
    char buf[128]={0};
    sprintf(buf,"ps -ax | grep %s|grep -v grep", process_name);
    if((strm = popen(buf, "r")) != NULL)
    {
        if(fgets(buf, sizeof(buf), strm) != NULL)
        {
            n = atoi(buf);
        }
    }
    else
    {
        return -1;
    }
    pclose(strm);
    return n;
}

#if 0
int main(){

    int serial_fd = -1;//串口文件描述符
    int len = 0;
    unsigned char buffer[6] = {0xAA,0x55,0x00,0x00,0x55,0xAA};
    char *category = NULL;//垃圾分类的结果
    int ret = -1;

    //初始化，python接口从哪里来？
    garbage_init();

    // ret = detect_process("mjpg_streamer");//用于判断mjpg_streamer服务是否已经启动
    // if ( -1 == ret)
    // {
    //     goto END;
    // }
    //利用串口来做香橙派，串口打开是否成功，能不能从串口读取到数据？
    serial_fd = myserialOpen(SERIAL_DEV,BAUD);
    if(-1 == serial_fd){
        goto END;
    }
    //怎么让香橙派读取语音模块的数据

    while(1){
        len = serialGetstring(serial_fd,buffer);
        //读取到语音模块的数据了吗？
        if(len > 0 && buffer[2] == 0x46){
            buffer[2] = 0x00;
            //让香橙派去拍照
            system(WGET_CMD);
            //判断照片是否存在
            if(0 == access(GARBAGE_FILE,F_OK)){
                //照片存在的话，阿里云识别之后会返回识别结果
                category = garbage_category(category);
                if(strstr(category,"干垃圾")){
                    buffer[2] = 0x41;
                }else if(strstr(category,"湿垃圾")){
                    buffer[2] = 0x42;
                }else if(strstr(category,"可回收垃圾")){
                    buffer[2] = 0x43;
                }else if(strstr(category,"有害垃圾")){
                    buffer[2] = 0x44;
                }else{
                    buffer[2] = 0x45;
                }
            }else{
                buffer[2] = 0x45;
            }
            if(buffer[2] != 0){
                printf("%d",buffer[2]);
            }else{
                printf("没有数据");
            }
            //把读到的数据通过串口发送给语音模块，语音播报
            serialSendstring(serial_fd,buffer,6);
            //发完把数据清空，等待下一次重新发送
            buffer[2] = 0x00;
            remove(GARBAGE_FILE);

        }
    }
    close(serial_fd);

END:
    //释放解释器
    garbage_final();
    return 0;
}
#endif

int main(){

    int ret = -1;
    int len = 0;
    char *category = NULL;

    pthread_t get_voice_tid,category_tid,get_socket_tid;

    wiringPiSetup();
    garbage_init();

    ret = detect_process("mjpg_streamer");
    if( -1 == ret)
    {
        printf("detect process failed\n");
        goto END;
    }

    serial_fd = myserialOpen(SERIAL_DEV,BAUD);
    if(-1 == serial_fd){
        goto END;
    }

    //开语音线程
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    pthread_create(&get_voice_tid,NULL,pget_voice,&len);

    //开网络线程get_socket_tid
    pthread_create(&get_socket_tid,NULL,pget_socket,&category);

    //开阿里云交互线程
    printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
    pthread_create(&category_tid,NULL,pcategory,&category);

    pthread_join(get_voice_tid,NULL);
    pthread_join(category_tid,NULL);
    pthread_join(get_socket_tid,NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    close(serial_fd);

END:
    garbage_final();
    return 0;
}



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
 
//打开串口并配置串口属性
int myserialOpen (const char *device, const int baud)
{
    struct termios options ;
    speed_t myBaud ;
    int status, fd ;
 
    switch (baud)
    {
      case    9600:	myBaud =    B9600 ; break ;
      case  115200:	myBaud =  B115200 ; break ;
    }
 
    //=======================================1. 打开串口=======================================
    fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    //将fd 的状态标记设置为可读写模式,确保文件描述符可以被读取和写入
    fcntl (fd, F_SETFL, O_RDWR) ;
 
    //=======================================2. 配置串口属性=======================================
    //获取当前串口的属性配置，并将其保存在 options 结构体中
    tcgetattr (fd, &options) ;
    //设置串口为原始模式
    cfmakeraw   (&options) ;
    cfsetispeed (&options, myBaud) ;
    cfsetospeed (&options, myBaud) ;
    //配置串口
    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;
    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] = 100 ;
 
    //=======================================3. 将修改后的串口属性立即应用到串口=======================================
    //将修改后的串口属性应用到串口
    tcsetattr (fd, TCSANOW, &options) ;
 
    //=======================================4. 设置串口为已经准备好发送和接受数据=======================================
    //获取当前串口控制信号的状态，并将状态值保存在变量 status 中
    ioctl (fd, TIOCMGET, &status);
    //控制信号设置为高电平状态，表示串口已经准备好发送和接收数据
    status |= TIOCM_DTR ;
    status |= TIOCM_RTS ;
    //将修改后的控制信号状态应用到串口，确保设备正确配置和准备好进行数据的发送和接收操作。
    ioctl (fd, TIOCMSET, &status);
 
    //=======================================5. 延时一段时间确保串口配置成功=======================================
    usleep (10000) ;// 10mS
    return fd ;
}
 
//关闭串口
void myserialClose (const int fd)
{
  close (fd) ;
}
 
 
//发送字符串
void serialSendstring (const int fd, const char *s)
{ 
	write (fd, s, strlen (s));
}
 
//接受字符串
int serialGetstring (const int fd,char *buffer)
{
  int n_read;
  n_read = read (fd, buffer, 32);
  return n_read;
}
 
 
 

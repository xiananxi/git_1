#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>

#define BEEP 0  //设置针脚0为蜂鸣器的控制引脚

int main (void)
{

	wiringPiSetup();//初始化wiringPi库
	pinMode (BEEP, OUTPUT); // 设置io口的输入输出，此处输出
	//实现循环响1s，停止响1s
	while(1){
		sleep(1);
		digitalWrite (BEEP, HIGH);	// 设置IO口输出低电平蜂鸣器响
		sleep(1);
    	digitalWrite (BEEP, LOW);   // 设置IO口输出高电平蜂鸣器停止响声
	}

	return 0;
}

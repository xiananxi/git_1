#ifndef _UARTTOOL_H
#define _UARTTOOL_H

int myserialOpen (const char *device, const int baud);
void serialSendstring (const int fd, const char *s,int len);
int serialGetstring (const int fd, char *buffer);

#define SERIAL_DEV "/dev/ttyS5"
#define BAUD 115200

#endif
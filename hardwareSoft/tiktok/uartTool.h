int myserialOpen (const char *device, const int baud);

void serialSendstring (const int fd, const char *s);

int serialGetstring (const int fd,char *readBuf);

char myserialGetchar (const int fd);

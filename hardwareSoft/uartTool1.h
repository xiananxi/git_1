int myserialOpen (const char *device, const int baud);
void myserialClose (const int fd);
void serialSendstring (const int fd, const char *s);
int serialGetstring (const int fd,char *buffer);
 
 

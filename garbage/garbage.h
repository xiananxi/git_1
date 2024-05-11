//garbage.h
#ifndef __GARBAGE__H
#define __GARBAGE__H
void garbage_init(void);
void garbage_final(void);
char *garbage_category(char *category);

#define WGET_CMD "wget http://127.0.0.1:8080/?action=snapshot -O /home/orangepi/tmp/garbage.jpg"
#define GARBAGE_FILE "/home/orangepi/tmp/garbage.jpg"

#endif
 
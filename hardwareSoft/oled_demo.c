/*
 * Copyright (c) 2015, Vladimir Komendantskiy
 * MIT License
 *
 * SSD1306 demo of block and font drawing.
 */

//
// fixed for OrangePiZero by HypHop
//

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "oled.h"
#include "font.h"

int oled_show(struct display_info *disp) {
	int i;
	char buf[100];

	oled_putstrto(disp, 0, 9+1, "Welcome to my HomeAssistant");
	disp->font = font2;

	oled_putstrto(disp, 0, 20, "        ---- xainanxi------");
	disp->font = font2;

	oled_send_buffer(disp);
	return 0;
}

void show_error(int err, int add) {
	//const gchar* errmsg;
	//errmsg = g_strerror(errno);
	printf("\nERROR: %i, %i\n\n", err, add);
	//printf("\nERROR\n");
}

void show_usage(char *progname) {
	printf("\nUsage:\n%s <I2C bus device node >\n", progname);
}

int main(int argc, char **argv) {
	int e;
	char filename[32];
	struct display_info disp;

	if (argc < 2) {
		show_usage(argv[0]);
		
		return -1;
	}

	memset(&disp, 0, sizeof(disp));
	sprintf(filename, "%s", argv[1]);
	disp.address = OLED_I2C_ADDR;
	disp.font = font2;

	e = oled_open(&disp, filename);
	
	e = oled_init(&disp);
	oled_show(&disp);

	return 0;
}



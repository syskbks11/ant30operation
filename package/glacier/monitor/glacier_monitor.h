#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>

#define cls() printf("\x1b[2J")


void glacier_monitor_setup_default();
void glacier_monitor_setup(int nRaw, int sHeaderRaw, int sUpperBarRaw, int sRaw, int sColomn, int sAreaFixed);

/* draw screen */
void setHeader(char* str);
void color(int c);
void setColor(int c);
void setBackgroundColor(int c);
void setLabel(char* str, int row, int column);

void setFixedLabel(char* str, int row, int column);
void setStatusBar(char* str);
void setHeader(char* str);
void setUpperBar(char* str);

/* utility */
int check_stdin_status(int sec, int usec);


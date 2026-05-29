#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>


#include "glacier_monitor.h"



#define LOOP_NUM 100
#define COLOR_NUM 8

/*
#define ROW_NUM 3


#define HEARDER_ROW_SIZE 1
#define UPPERBAR_ROW_SIZE 1

#define COLUMN_SIZE 30
#define ROW_SIZE 1
#define AREA_F_SIZE 13
#define AREA_C_SIZE (COLUMN_SIZE-AREA_F_SIZE)
#define LABEL_F_SIZE (AREA_F_SIZE-INSERT_F_SIZE)
#define LABEL_C_SIZE (AREA_C_SIZE-INSERT_C_SIZE)
*/
#define INSERT_F_SIZE 1
#define INSERT_C_SIZE 1

/*
::123456789012345678901234567890123456789012345678901234567890
-----------------------------------------------------
1:HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
2:UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
3:AAAAAAAAAAAAA.aaaaaaaaaaaaaaa.BBBBBBBBBBBBB.bbbbbbbbbbbbbbbb.
4:CCCCCCCCCCCCC.ccccccccccccccc.DDDDDDDDDDDDD.dddddddddddddddd.
5:EEEEEEEEEEEEE.eeeeeeeeeeeeeee.FFFFFFFFFFFFF.ffffffffffffffff.
6:SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
*/


#define gotoxy(x,y) printf("\x1b[%d;%dH",(y+1),(x+1))

typedef struct glacier_monitor_s{
	int nRaw;
//	int nColumn;
	int sHeaderRaw;
//	int sHeaderColumn;
	int sUpperBarRaw;
	int sColumn;
	int sRaw;
	int sAreaFixed;
//	int sAreaChange:
//	int sInsertFixed;
//	int sInsertChange;
	int sLabelFixed;
	int sLabelChange;
}glacier_monitor_t;

static glacier_monitor_t p;

void glacier_monitor_setup_default(){
	memset(&p, 0, sizeof(glacier_monitor_t));
	p.nRaw = 3;
	p.sHeaderRaw = 1;
	p.sUpperBarRaw = 1;
	p.sColumn = 30;
	p.sRaw = 1;
	p.sAreaFixed = 13;
	p.sLabelFixed = p.sColumn-p.sAreaFixed-INSERT_F_SIZE;
	p.sLabelChange = p.sAreaFixed-INSERT_C_SIZE;
}
void glacier_monitor_setup(int nRaw, int sHeaderRaw, int sUpperBarRaw, int sRaw, int sColomn, int sAreaFixed){
	memset(&p, 0, sizeof(glacier_monitor_t));
	p.nRaw = 3;
	p.sHeaderRaw = 1;
	p.sUpperBarRaw = 1;
	p.sColumn = 30;
	p.sRaw = 1;
	p.sAreaFixed = 13;
	p.sLabelFixed = p.sColumn-p.sAreaFixed-INSERT_F_SIZE;
	p.sLabelChange = p.sAreaFixed-INSERT_C_SIZE;
}

void color(int c)
{
	//remember the previous color.
	static int previous = -1;
	if(previous != c){

		//What we want to do.
		printf("\x1b[%dm", 30+c);

		previous = c;
	}
}

void setColor(int c){
	printf("\x1b[%dm", 30+c);
}

void setBackgroundColor(int c){
	printf("\x1b[%dm", 40+c);
}

void setLabel(char* str, int row, int column)
{
	gotoxy(column*p.sColumn+p.sAreaFixed, row*p.sRaw+p.sHeaderRaw+p.sUpperBarRaw);
	int len=strlen(str);
	if(len > p.sLabelChange){
		str[p.sLabelChange]='\0';
	}
	printf(str);
	fflush(stdout);
}

void setFixedLabel(char* str, int row, int column)
{
	gotoxy(column*p.sColumn, row*p.sRaw+p.sHeaderRaw+p.sUpperBarRaw);
	int len=strlen(str);
	if(len > p.sLabelFixed){
		str[p.sLabelFixed]='\0';
	}
	printf(str);
	fflush(stdout);
}

void setStatusBar(char* str)
{
	gotoxy(0, p.nRaw*p.sRaw+p.sHeaderRaw+p.sUpperBarRaw);	
	printf(str);
	fflush(stdout);
}
void setHeader(char* str)
{
	gotoxy(0, 0);	
	printf(str);
	fflush(stdout);
}
void setUpperBar(char* str)
{
	gotoxy(0, p.sHeaderRaw);	
	printf(str);
	fflush(stdout);
}


int check_stdin_status(int sec, int usec){
    fd_set rfds;
    struct timeval tv;
    int retval;

    /* stdin (fd 0) を監視し、入力があった場合に表示する。*/
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    /* sec 秒間監視する。*/
    tv.tv_sec = sec;
    tv.tv_usec = usec;

    retval = select(1, &rfds, NULL, NULL, &tv);
    /* この時点での tv の値を信頼してはならない。*/

	if (retval == -1){
		perror("select()");
		return -1;
	}
	if (retval){
		//printf("今、データが取得できました。\n");
		/* FD_ISSET(0, &rfds) が true になる。*/
		return 1;
	}
        //printf("%d.%06d 秒以内にデータが入力されませんでした。\n", sec, usec);
	return 0;
}




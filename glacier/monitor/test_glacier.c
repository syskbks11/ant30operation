#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>

#include "glacier_monitor.h"


static void draw_changableLabels(char* str00, char* str10, char* str20, char* str01, char* str11, char* str21);
static void draw();
static void draw_fixedLabels();

void draw()
{
	draw_fixedLabels();

}

void draw_fixedLabels(){
	setHeader("A TEST MONITOR (2008/12/22)");
	
	setColor(1);
	setFixedLabel("cat", 0, 0);
	setBackgroundColor(5);
	setFixedLabel("dog", 1, 0);
	setFixedLabel("fox", 2, 0);
	setBackgroundColor(7);
/*
	setColor(2);
	setFixedLabel("cat2", 0, 1);
	//setBackgroundColor(6);
	setFixedLabel("dog2", 1, 1);
	setFixedLabel("fox2", 2, 1);
	//setBackgroundColor(7);
*/
	setColor(0);
}

void draw_changableLabels(char* str00, char* str10, char* str20, char* str01, char* str11, char* str21){
	setColor(3);
	setLabel(str00, 0, 0);
	setLabel(str10, 1, 0);
	setLabel(str20, 2, 0);
/*
	setColor(4);
	setLabel(str01, 0, 1);
	setLabel(str11, 1, 1);
	setLabel(str21, 2, 1);
*/
	setColor(0);
}



main()
{
	int c='a';

//	glacier_monitor_setup_default();
	glacier_monitor_setup(3, 1, 1, 1, 20, 10);

	initscr();
	raw();
	noecho();
	
	char tmpstr[256];    // データ受信バッファ

	char date[128];
	double dat1;
	double dat2;
	double dat3;

	char str00[10];
	char str10[10];
	char str20[10];
	char str01[10] = "0.01";
	char str11[10] = "0.02";
	char str21[10] = "0.03";

	memset(str00, 0, 10);
	memset(str10, 0, 10);
	memset(str20, 0, 10);

	char msg[128];
	char* ret_fgets;
	int ret_sscanf;

	draw();

	int i = 0;
	int isContinue = 1;
	while(isContinue){
		int retval = check_stdin_status(0, 1000*100);
		switch(retval){
		case 1:
			ret_fgets = fgets(tmpstr, 256, stdin);
			if(ret_fgets == NULL){
				isContinue = 0;
				sprintf(msg, "stdin closed.          ");
				break;
			}
			sprintf(msg, "I've got from stdin.          ");
			setStatusBar(msg);
			ret_sscanf = sscanf(tmpstr, "%s\t%lf\t%lf\t%lf\n", date, &dat1, &dat2, &dat3);
			if(ret_sscanf < 4){
				sprintf(msg, "\n%s", tmpstr);
				setStatusBar(msg);
				break;
			}
			sprintf(str00, "% 7.2lf", dat1);
			sprintf(str10, "% 7.2lf", dat2);
			sprintf(str20, "% 7.2lf", dat3);
			draw_changableLabels(str00, str10, str20, str01, str11, str21);
/*
			sprintf(msg, "%s %lf %lf %lf\n", date, dat1, dat2, dat3);
			setStatusBar(msg);
*/
			setUpperBar(date);
			break;
		case -1:
			sprintf(msg, "Problem in reading data.         ");
			setStatusBar(msg);
			break;
		case 0:
			sprintf(msg, "I've got no data from stdin.     ");
			setStatusBar(msg);
			break;
		default:
			break;
		}

		i++;
	}
	usleep(1000*1000*3);
	setStatusBar("I' exit in 3 seconds....     ");
	usleep(1000*1000*3);

	endwin();
}

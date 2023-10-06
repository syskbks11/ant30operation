#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>


#define LOOP_NUM 100
#define COLOR_NUM 8

#define ROW_NUM 3
#define COLUMN_NUM 2

#define COLUMN_SIZE 20
#define ROW_SIZE 1
#define AREA_F_SIZE 9
#define AREA_C_SIZE (COLUMN_SIZE-AREA_F_SIZE)
#define INSERT_F_SIZE 1
#define INSERT_C_SIZE 1
#define LABEL_F_SIZE (AREA_F_SIZE-INSERT_F_SIZE)
#define LABEL_C_SIZE (AREA_C_SIZE-INSERT_C_SIZE)

#define gotoxy(x,y) printf("\x1b[%d;%dH",(y+1),(x+1))
#define cls() printf("\x1b[2J")


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

void setColor(int c)
{
	printf("\x1b[%dm", 30+c);
}

void setBackgroundColor(int c)
{
	printf("\x1b[%dm", 40+c);
}

void setLabel(char* str, int row, int column)
{
	gotoxy(column*COLUMN_SIZE+AREA_F_SIZE, row*ROW_SIZE);
	int len=strlen(str);
	if(len > LABEL_C_SIZE){
		str[LABEL_C_SIZE]='\0';
	}
	printf(str);
	fflush(stdout);
}

void setFixedLabel(char* str, int row, int column)
{
	gotoxy(column*COLUMN_SIZE, row*ROW_SIZE);
	int len=strlen(str);
	if(len > LABEL_F_SIZE){
		str[LABEL_F_SIZE]='\0';
	}
	printf(str);
	fflush(stdout);
}
void setStatusBar(char* str)
{
	gotoxy(0, ROW_NUM);	
	printf(str);
	fflush(stdout);
}

void draw()
{
	setColor(1);
	setFixedLabel("cat", 0, 0);
	setBackgroundColor(5);
	setFixedLabel("dog", 1, 0);
	setFixedLabel("fox", 2, 0);
	setBackgroundColor(7);

	setColor(2);
	setFixedLabel("cat2", 0, 1);
	setBackgroundColor(6);
	setFixedLabel("dog2", 1, 1);
	setFixedLabel("fox2", 2, 1);
	setBackgroundColor(7);

	setColor(3);
	setLabel("0.01", 0, 0);
	setLabel("0.02", 1, 0);
	setLabel("-0.03", 2, 0);

	setColor(4);
	setLabel("-0.01", 0, 1);
	setLabel("-0.02", 1, 1);
	setLabel(" 0.03", 2, 1);

	color(0);
	setStatusBar("Normal Information. ");
	//usleep(2000*1000);

}
main()
{
	//char str[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	//int len=strlen(str);
	int c='a';

	initscr();
	raw();
	//noecho();
	
	//cls();

	//draw();
	printf("Start monitor: push a key!.");
	fflush(stdout);
	usleep(1000*1000);
	do{
		switch(c){
		//case '1':
		default:
			draw();
			break;
		}
	}while( (c=getch()) !='.');

	//usleep(1000*1000);

	endwin();
}

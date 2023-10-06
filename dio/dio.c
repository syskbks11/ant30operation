/*******************************************************************************
 *                 Eagle Technology - Cape Town, South Africa
 *             Copyright (C) 2004 - 2005 - All Rights Reserved!
 *                 www.eagledaq.com - eagle@eagle.co.za
 *******************************************************************************
 *
 * dio.c - Digital I/O Example
 *
 *******************************************************************************
 *                              LICENSE AGREEMENT
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *******************************************************************************
 * Written by Jan Zwiegers - jan@eagle.co.za
 *
 * Version 1.0 - Original Release
 *
 ******************************************************************************/

/*
 * Modified by Nagai Makoto - nagai@sakura.cc.tsukuba.ac.jp
 * 2008/9/24 format '%ld' is changed to '%d' for int. 
 * 2008/9/24 function Monitor() is added. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <term.h>
#include <termios.h>
#include <edrapi.h>
#include <query.h>
#include <boards.h>
#include <errors.h>
#define STRLEN 5

int fh;
char tmpstr[STRLEN+1];
static struct termios old_set,new_set;
unsigned int *parg;

void init_keyboard(){
    tcgetattr(0,&old_set);
    new_set=old_set;
    new_set.c_lflag &=~ICANON;
    new_set.c_lflag &=~ECHO;
    new_set.c_lflag &=~ISIG;
    new_set.c_cc[VMIN] = 1;
    new_set.c_cc[VTIME] = 0;
    tcsetattr(0,TCSANOW,&new_set);
}

void close_keyboard(){
    tcsetattr(0,TCSANOW,&old_set);
}

void ShowError(long err)
{
    char s[40];
    EDRE_StrError(err,s);
    printf("\n%s\n",s);
}

void Info(unsigned long sn)
{
    unsigned long md[3];
    //long ret;
    char bn[40];

    printf("\n");
    printf("Board Information\n");
    printf("-----------------\n");
    printf("Serial Number = %ld\n",sn);    
    EDRE_StrBoardName(sn,bn);
    printf("Board Name = %s\n",bn);
    printf("Board Type = %ld\n",EDRE_Query(sn,BRDTYPE,0));
    printf("Board Revision = %ld\n",EDRE_Query(sn,BRDREV,0));
    printf("Number of digital I/O port = %ld\n",EDRE_Query(sn,DIONUMPORT,0));    
    md[0]=EDRE_Query(sn,BRDDAY,0);
    md[1]=EDRE_Query(sn,BRDMONTH,0);
    md[2]=EDRE_Query(sn,BRDYEAR,0);    
    printf("Manufacturing Date = %ld/%ld/%ld\n",md[0],md[1],md[2]);    
    md[0]=EDRE_Query(sn,DRVMAJOR,0);
    md[1]=EDRE_Query(sn,DRVMINOR,0);
    md[2]=EDRE_Query(sn,DRVBUILD,0);    
    printf("Driver Version = %ld.%ld.%ld\n",md[0],md[1],md[2]);    
}

void Read(unsigned long sn)
{
    unsigned long port,value;    
    long err;    
    printf("\n");
    printf("Read from a Port\n");
    printf("----------------\n");
    printf("Port: ");
    fgets(tmpstr,STRLEN,stdin);
    port=atoi(tmpstr);
    err=EDRE_DioRead(sn,port,&value);
    if(err<EDRE_OK) ShowError(err);
    else printf("Value = %ld\n",value);
}

void Monitor(unsigned long sn)
{
    unsigned long portFirst,portLast,value;    
    long err;    
    printf("\n");
    printf("Monitor Ports\n");
    printf("----------------\n");
    printf("First Port: ");
    fgets(tmpstr,STRLEN,stdin);
    portFirst=atoi(tmpstr);
    printf("Last Port: ");
    fgets(tmpstr,STRLEN,stdin);
    portLast=atoi(tmpstr);
	while(1){
	int i;	
	for (i = portFirst; i <= portLast ; i++ ){
		printf("P%d\t",i);
	}
	printf("\n");
	for (i = portFirst; i <= portLast ; i++ ){
		err=EDRE_DioRead(sn, i, &value);
		if(err<EDRE_OK) ShowError(err);
		else printf("%ld\t",value);
	}
	printf("\n");
	usleep(1000*1000);
	}
}

void Write(unsigned long sn)
{
    unsigned long port,value;    
    long err;    
    printf("\n");
    printf("Write to a Port\n");
    printf("----------------\n");
    printf("Port: ");
    fgets(tmpstr,STRLEN,stdin);
    port=atoi(tmpstr);
    printf("Value: ");
    fgets(tmpstr,STRLEN,stdin);
    value=atoi(tmpstr);
    err=EDRE_DioWrite(sn,port,value);
    if(err<EDRE_OK) ShowError(err);
}

int main(int argc, char **argv){
    char ch,s[40];
    int bn,di,i;
    unsigned long sn;
    
    printf("*** Digital I/O Monitor Program ***\n");
    printf("Eagle Technology (C) 2001, Nagai 2008\n");

    //Query number of installed devices
    di=EDRE_Query(0,APINUMDEV,0);
    printf("Devices installed = %d\n",di);
    if(di==0) exit(0);
    
    for(i=0;i<di;i++)
    {
	//get serial number of each installed device
	sn=EDRE_Query(0,BRDSERIALNO,i);
	//get text name of each device 
	EDRE_StrBoardName(sn,s);
	printf("%d) %ld - %s\n",i,sn,s);    	
    }
    
    do
    {
	printf("Select a board: ");
	fgets(tmpstr,STRLEN,stdin);
	bn=atoi(tmpstr);
	if(bn>=di) printf("Device number invalid, try again.\n");
    }while(bn>=di);
    sn=EDRE_Query(0,BRDSERIALNO,bn);
    do{
	printf("\n=== Menu ===\n");
	printf("I.  Display board information.\n");
	printf("R.  Read from a port.\n");
	printf("M.  Monitor ports.\n");
	printf("W.  Write to a port.\n");
	printf("Esc. Exit.\n");	
	init_keyboard();
	read(0,&ch,1);
	close_keyboard();
	
	switch(ch){
	case 'r':
	case 'R':
		Read(sn);
		break;
	case 'm':
	case 'M':
		Monitor(sn);
		break;
	case 'w':
	case 'W':	
		Write(sn);
		break;
	case 'i':
	case 'I':
		Info(sn);
		break;
	}
		
    }while(ch!=27);
    
    exit(0);    
}

/**
 * End of File
 */

/*!
¥file instrumentTest.cpp
¥author NAGAI Makoto
¥date 2009.2.25
¥brief Manage Instrument parameter for 32-m telescope & 30-cm telescope
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "configuration.h"
#include "instrument.h"


int manageCommand(char* cmd){
	double x;
	double y;

	uM1("cmd: %s", cmd);
	if(cmd[0] == 'e'){
		return 0;
	}
	int ret = sscanf(cmd, "%lf %lf", &x, &y);
	if(ret < 0)
		return 1;

	double dx;
	double dy;
	instrumentCalInst(x, y, &dx, &dy);
	return 1;
}
int main(){
	confInit();
	confAddFile("../etc/ant30.conf");

	instrumentInitParam();

	char tmpstr[256];    // データ受信バッファ
	int cont = 1;
	int n = 1;

	do{
		
		/* print prompt */
		printf("[instrumentTest: %d] > ", n);
		fflush(stdout);

		fgets(tmpstr, 256, stdin);
		cont = manageCommand(tmpstr);
		n++;
	}while(cont);

	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_gauge.h"


static void show_Help();
static int test_by_manual();

void show_Help(){
	printf("%s\n", "This is help of \"gauge\". ");
	printf("%s\n", "Available commands: ");
	printf("%s\n", "\te, exit: ");
	printf("%s\n", "\t\tExit the program. ");
	printf("%s\n", "\thelp: ");
	printf("%s\n", "\t\tShow this document. ");
	printf("%s\n", "\tp, pressure: ");
	printf("%s\n", "\t\tGet pressure now. ");
	printf("%s\n", "\ti, identification: ");
	printf("%s\n", "\t\tget the kind of the gauge. ");
	printf("%s\n", "\t?: ");
	printf("%s\n", "\t\tget error status. ");
}

int test_by_manual(){
	int len;                            //  受信データ数（バイト）
	char tmpstr[256];    // データ受信バッファ
	int res, pact, vact;

	printf("%s\n", "Test start. ");
	int n = 0;
	
	int cont = 1;    


	do{
		n++;
		printf("[gauge: %d] > ", n);
		fflush(stdout);
		//printf("Esc");	
		//init_keyboard();
	
		fgets(tmpstr, 256, stdin);
		//key = fgetc(stdin)
		//read(0,&ch,1);
		//close_keyboard();

		printf("cmd:%s\n", tmpstr);
	
		long pos;
		char *token;
		double pressure;
		switch(tmpstr[0]){
		case 'p':
			pressure = penguin_gauge_get_pressure(1);
			printf("pressure: %e\n", pressure);
			break;
		case 'i':
			penguin_gauge_get_identification();
			printf("id: %s\n", penguin_gauge_getMessage());
			break;
		case 'e':
			cont = 0;
			break;
		case 'h':
			if(strncmp("help", tmpstr, 4)==0){
				show_Help();
			}
			break;
		case '?':
			res = penguin_gauge_get_errorStatus();
			printf("status: %d\n", res);
			break;

		default:break;	
		}
		
	}while(cont);



	printf("%s\n", "Test end. ");

	return 0;
	
}


int	main(int argc, char *argv[]) {


	printf("%s\n", "This software is \"gauge\", a test program for single gauge of Pfeiffer.");	
	printf("%s\n", "If you need more information, please type \"help\". ");	

	if(penguin_gauge_init()){
		printf("penguin_gauge_init() error.");
		exit(1);
	}


	test_by_manual();

	penguin_gauge_end();
}

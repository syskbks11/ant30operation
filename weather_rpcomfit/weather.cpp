#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_chopper.h"

int test_by_manual(){
	int len;                            //  受信データ数（バイト）
	char tmpstr[256];    // データ受信バッファ
	int res, pact, vact;

	printf("%s\n", "Test start. ");
	int n = 0;
	
	int cont = 1;    
	do{
	n++;
	printf("[chopper: %d] > ", n);
	fflush(stdout);
	//printf("Esc");	
	//init_keyboard();
	
	fgets(tmpstr, 256, stdin);
	//key = fgetc(stdin)
	//read(0,&ch,1);
	//close_keyboard();

	pact = penguin_chopper_getPulse();
	printf("[Q:A1]%d\n",pact);

//	state_switches = Read(sn);
	//printf("PACT, VACT = %d %d\n",pact, vact);
	//printf("PACT, VACT, SW = %d %d %d\n",pact, vact, state_switches);
//	limitState(fd, sn);
	printf("cmd:%s", tmpstr);
	
	long pos;
	char *token;
	switch(tmpstr[0]){
	case 'o':
		penguin_chopper_open();
		break;
	case 'c':
		penguin_chopper_close();
		break;
	case 'i':
		penguin_chopper_configure_settings();
		break;
	case 'x':
		token=strtok(tmpstr," ");
		if(token == NULL)break;
		printf("token%s", token);
		token=strtok(NULL," ");
		if(token == NULL)break;
		pos = strtod(token, NULL);
		penguin_chopper_setPulse(pos);
		break;
	case 's':
		penguin_chopper_stop();
		break;
	case 'e':
		cont = 0;
		break;
	case '#':
		break;

	default:break;	
	}
		
    }while(cont);



	printf("%s\n", "Test end. ");

	return 0;
	
}


int	main(int argc, char *argv[]) {

	if(penguin_chopper_init()){
		printf("penguin_chopper_init() error.");
		exit(1);
	}


	test_by_manual();

	penguin_chopper_end();
}

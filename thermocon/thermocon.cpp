#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_thermocon.h"

void showHelp(){
	printf("%s\n", "This is a test program to communicate with Thermo-con, SMC. ");	
	printf("%s\n", "(commands)");	
	printf("\t%s\n", "e:");	
	printf("\t\t%s\n", "End the program.");	
	printf("\t%s\n", "h:");	
	printf("\t\t%s\n", "Show this message.");	
	printf("\t%s\n", "p:");	
	printf("\t\t%s\n", "Set preset temperature.");	
	printf("\t%s\n", "i:");	
	printf("\t\t%s\n", "Get inside temperature.");	
	printf("\t%s\n", "o:");	
	printf("\t\t%s\n", "Get outside temperature.");	
	printf("\t%s\n", "#:");	
	printf("\t\t%s\n", "Get preset temperature.");	
}

int test_by_manual(){
	int len;                            //  受信データ数（バイト）
	char tmpstr[256];    // データ受信バッファ
	//int res, pact, vact;

	printf("%s\n", "Test start. ");
	int n = 0;
	
	int cont = 1;    
	do{
	n++;
	printf("[thermocon: %d] > ", n);
	fflush(stdout);
	
	fgets(tmpstr, 256, stdin);

	printf("cmd:%s", tmpstr);
	
	int value, num;
	float preset = 20;
	char token[32];
	
	switch(tmpstr[0]){
	case 'p':
		num = sscanf(tmpstr, "%s %f", token, &preset);
		if(num == 2){
			penguin_thermocon_send_set_command(0x31, (int)preset*10);
		}
		penguin_thermocon_receive();
		break;
	case 'o':
		value = penguin_thermocon_getOutsideTemperature();
		printf("outside: %2.2f [degC]\n", value/100.0);
		break;
	case 'i':
		value = penguin_thermocon_getInsideTemperature();
		printf("inside: %2.2f [degC]\n", value/100.0);
		break;
	case 'a':
		value = penguin_thermocon_getAlerm();
		printf("alerm: %d\n", value);
		break;
	case 'e':
		cont = 0;
		break;
	case '#':
		value = penguin_thermocon_getPresetTemperature();
		printf("preset: %2.1f [degC]\n", value/100.0);
		break;
	case 'h':
		showHelp();
		break;

	default:break;	
	}
		
    }while(cont);



	printf("%s\n", "Test end. ");

	return 0;
	
}


int	main(int argc, char *argv[]) {

	if(penguin_thermocon_init(argv[1])){
		printf("penguin_thermocon_init() error.");
		exit(1);
	}


	test_by_manual();

	penguin_thermocon_end();
}

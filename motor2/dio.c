#include <edrapi.h>
#include <query.h>
#include <boards.h>
#include <errors.h>

#include "dio.h"


void ShowError(long err)
{
    char s[40];
    EDRE_StrError(err,s);
    printf("\n%s\n",s);
}


unsigned long init_DIO()
{

	char s[40];
	int bn, di, i;
	unsigned long sn;

    di=EDRE_Query(0,APINUMDEV,0);
    printf("Devices installed = %d\n",di);
//    if(di==0) exit(0);
    if(di==0) return 0;
    
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
	bn = 0;	
	printf("board %d is used: ", bn);
	//fgets(tmpstr,STRLEN,stdin);
	//bn=atoi(tmpstr);
	//if(bn>=di) printf("Device number invalid, try again.\n");
    }while(bn>=di);
    sn=EDRE_Query(0,BRDSERIALNO,bn);

	return sn;

}

unsigned long Read(unsigned long sn)
{
    unsigned long port,value;    
    long err;    
    //printf("\n");
    //printf("Read from a Port\n");
    //printf("----------------\n");
    //printf("Port: ");
    //fgets(tmpstr,STRLEN,stdin);
    //port=atoi(tmpstr);
	port = 0;
	//printf("Read from Port %d\n", port);
    err=EDRE_DioRead(sn,port,&value);
    if(err<EDRE_OK) ShowError(err);
    else {
	//printf("Value = %ld\n",value);
	}
	return value;
}


#include <stdlib.h>//exit()
#include <unistd.h>//usleep()

#include "configuration.h"
#include "acu.h"
#include "errno.h"



int main(int argc,char *argv[]){

	/* to use libtkb */
	uInit("../log/acuTest");
	uM("Initialization.");
		//!パラメーター読込み
	confInit();
	confAddFile("../../tcs01/etc/ant30.conf");

	
	/* test */
	acuInit();

	acuSafetyCheck();

	acuSetDoAzC(0);
	acuSetDoElC(0);
	
	acuSetACU(0);

	usleep(1000*1000);

	acuSafetyCheck();

	acuSetDoAzC(90);
	acuSetDoElC(90);
	
	acuSetACU(0);

	acuEnd();

	/* to use libtkb */
	uEnd();

	exit(0);
}


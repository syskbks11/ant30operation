#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>



int
main(void)
{
	char tmpstr[256];
	int i = 0;

	int hour = 1;
	int minute = 1;
	int sec = 1;
	double dat1 = 0.1;
	double dat2 = 0.1;
	double dat3 = 0.1;
	while(i < 100){
		sprintf(tmpstr, "%04d/%02d/%02d-%02d:%02d:%02d %f\t%f\t%f\n", 1492, 10, 12, hour, minute, sec, dat1, dat2, dat3);
//		sprintf(tmpstr, "%04d/%02d/%02d-%02d:%02d:%02d\t%f\t%f\t%f\n", 1492, 10, 12, hour, minute, sec, dat1, dat2, dat3);
		printf(tmpstr);
		fflush(stdout);
		usleep(1000*500);
		i++;

		if(sec == 59){
			sec = 0;
			minute ++;
		}else{
			sec ++;
		}

		if(i % 5 == 0){
			sprintf(tmpstr, "%04d/%02d/%02d-%02d:%02d:%02d\t%s\n", 1492, 10, 12, hour, minute, sec, "funny message.");
			printf(tmpstr);
			fflush(stdout);
		}
		dat1 += 0.1;
		dat2 *=1.1;
	}
	exit(EXIT_SUCCESS);
}


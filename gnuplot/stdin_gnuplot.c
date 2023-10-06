#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>


int
main(int argc,char *argv[])
{
	FILE* gnuplot = popen("/usr/bin/gnuplot", "w");
	if(gnuplot == NULL){
		return 1;
	}
	FILE* dat = fopen("/var/tmp/stdin_gnuplot.txt", "w");
	if(dat == NULL){
		return 1;
	}
	char tmpstr[256];    // データ受信バッファ
	struct timeval start;
	int ret = gettimeofday(&start, NULL);
	if(ret){
		return 1;
	}
	struct timeval now;
	struct timeval duration;
	double spent;//[sec]
/*
	system("echo '0 0' > /var/tmp/stdin_gnuplot.trm.txt");
	fprintf(gnuplot, "%s\n", "plot \"/var/tmp/stdin_gnuplot.trm.txt\" u 1:2");
*/
	fprintf(gnuplot, "%s\n", "set term x11");
	fprintf(gnuplot, "%s\n", "plot '< /usr/bin/tail -n 4000 /var/tmp/stdin_gnuplot.txt'");
	while(1){
		char* retc = fgets(tmpstr, 256, stdin);
		if(retc == NULL){
			break;
		}
		int ret = gettimeofday(&now, NULL);
		if(ret){
			break;
		}
		timersub(&now, &start, &duration);
		spent = (double)duration.tv_sec + (double)(duration.tv_usec * 1e-6);
		fprintf(dat, "%lf %s\n", spent, tmpstr);
		fflush(dat);

//		system("/usr/bin/tail -n 1500 /var/tmp/stdin_gnuplot.txt > /var/tmp/stdin_gnuplot.trm.txt");
		fprintf(gnuplot, "%s\n", "replot");

		fflush(gnuplot);
	}
	fflush(dat);
	fflush(gnuplot);
	usleep(1000*1000*1);
	return 0;
}

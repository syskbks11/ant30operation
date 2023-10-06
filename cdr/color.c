#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LOOP_NUM 100
#define COLOR_NUM 8

void color(int c)
{
	printf("\x1b[%dm", 30+c);
}

main()
{
	char str[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int len=strlen(str);
	int i;

	for(i=0; i<len*LOOP_NUM; i++){
		color(i % COLOR_NUM);
		putchar(str[i%len] );//do not work on debhykonyan
		//fputc(str[i%len], stdout);//do not work on debhykonyan
		fflush(stdout);
		//printf("%d", str[i%len]);
		//printf("%c", str[i%len]);
		usleep(1*1000);
		if(i%len==(len-1)){
			putchar('\r');
			//putchar('\n');
			usleep(1*1000);
		}
	}
	color(0);
}

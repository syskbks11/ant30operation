#include <stdio.h>

main()
{

 int i;

 printf("\x1b[2J");
 for ( i=0;i<40;i++){
    printf("\x1b[%d;%dH\x1b[7m%d\x1b[0m",i%10+1,i+1,i);
 }
 printf("\n");
}

#include <stdio.h>
 main()
{
 char line[10];
 int i;
 
 printf("Input one line:");
 fgets(line,sizeof(line),stdin);
 for(i=0;i<10;i++){
   printf("%d:%c(%d)\n",i,line[i],line[i]);
 }
}

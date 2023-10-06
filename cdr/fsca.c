#include <stdio.h>

void

main()
{
 FILE *fp;
 int i,n,m,l,sum;

 if( (fp=fopen("data","r")) == NULL){
     fputs("dataファイルがオープンできません。\n",stderr);
 }else{
   sum = 0;
   i = 0;
   while( fscanf( fp, "%d %d %d *[^\n]",&n,&m,&l) != EOF){
       i++;
       sum += n;
       printf("%d(%d):%d (%d)%d\n",i,m,n,sum,l);
   }
 }
}

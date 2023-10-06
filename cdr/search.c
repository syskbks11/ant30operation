#include <stdio.h>
#include <string.h>

#define USAGE "使い方:search {検索文字} {ファイル名}\n"
#define MAXLEN 2048

int search_file( char *pattern, char *fname);

void

main( int argc, char *argv[])
{

 int n;

 if( argc !=3){
    fputs(USAGE,stderr);
 }else{
    n = search_file(argv[1],argv[2]);
    printf("計%3d行\n",n);
 }
}

int

search_file( char *pattern, char *fname)
{
 FILE *fp;
 char line[MAXLEN];
 int l=0;
 int n=0;

 if( (fp=fopen(fname,"r")) == NULL){
      fprintf(stderr,"%sがオープンできません。\n",fname);
 }else{
   while( fgets ( line,sizeof(line),fp) != NULL){
          l++;
          if( strstr( line,pattern) != NULL){
                 printf("%4d : %s",l,line);
                 n++;
          }
   }
 fclose(fp);
 }
 return (n);
}

#include <stdio.h>
#include <string.h>

#define USAGE "�Ȥ���:search {����ʸ��} {�ե�����̾}\n"
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
    printf("��%3d��\n",n);
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
      fprintf(stderr,"%s�������ץ�Ǥ��ޤ���\n",fname);
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

#include	<stdio.h>
#include	<string.h>

#include	<profhead.h>
#include	<starlib.h>

#define	SEPS	" ,\t\n"

/*
update history

1992/12/21 T.Handa
*/


int	index_headin(default_sdbfile)
/*
Get default filename
1992/12/21 T.Handa
*/
char	*default_sdbfile;
{
	char	file_id[10];
	int	l_str;

/* get data idnetifier */
	if(fgets(file_id,10,stdin)==NULL)
	{
		return(1);
	}
	file_id[8]=NULL;
	if(strcmp(file_id,"STAR.sdb")!=0)
		return(999);

/* get default file name */
	if(fgets(default_sdbfile,MAX_FILE_NAME_LENGTH,stdin)==NULL)
	{
		return(999);
	}
	l_str=strlen(default_sdbfile)-1;
	default_sdbfile[l_str]=NULL;
	return(0);
}

int	index_headout(default_sdbfile)
/*
Put default filename
1992/12/21 T.Handa
*/
char	*default_sdbfile;
{
/* put data idnetifier */
	printf("STAR.sdb\n");

/* put default file name */
	printf("%s\n",default_sdbfile);
	return(0);
}



int	index_load(fp_index, filename, flag)
FILE	*fp_index;
char	*filename;
unsigned short	*flag;

/*	

機能）
fp_index に "rb" mode でオープンされた scan index file から一行を読み、ファイル名、フラッグを代入する。
返値）
	0	:	正常終了
	11	:	blank line
	100	:	EOF を検出した
	201	:	field の数が正しくない（少ない）
	202	:	field の数が正しくない（多い）
	301	:	flag の表示が正しくない

1992/12/1	岡　朋治
1992/12/11 半田利弘 (ファイル名のc-string化）
*/

{
char	buffer[MAX_IDX_RECORD_LENGTH];
char	filename_buff[MAX_IDX_RECORD_LENGTH];
char	flag_buff[MAX_IDX_RECORD_LENGTH];
char	*token;

/*	ストリームからの一行入力	*/

if(fgets(buffer,MAX_IDX_RECORD_LENGTH,fp_index)==NULL)	return(100);

/*	入力した文字列をトークンに区切る	*/
token=strtok(buffer,SEPS);
strcpy(filename_buff,token);
strncpy(filename,filename_buff,strlen(filename_buff));
if(filename[0]==NULL) return(11);
token=strtok(NULL,SEPS);
filename[strlen(filename_buff)]=NULL;
if(token==NULL){
	*flag=0;
	return(201);
	
}
strcpy(flag_buff,token);
token=strtok(NULL,SEPS);

if(token!=NULL){
	return(202);
}	

/*	flag 部分の識別		*/

if(	(strncasecmp(flag_buff,"g",1)==0)||
	(strncmp(flag_buff,"0",1)==0)){
	*flag=0;
	return(0);
}
else if((strncasecmp(flag_buff,"b",1)==0)||
	(strncmp(flag_buff,"1",1)==0)){
	*flag=1;
	return(0);
}
else	{
	*flag=1;
	return(301);
}

}

int	index_save(fp_index, filename, flag)
FILE	*fp_index;
char	*filename;
unsigned short	flag;

/*	

機能）
fp_index に "wb" mode でオープンされた scan index file へファイル名、フラッグ値を一行書き込む。
戻り値）
	0	:	正常終了
	101	:	flag の値が正しくない。(0 or 1 以外)

1992/12/1	岡　朋治

*/

{

if(flag==0){
	fprintf(fp_index,"%s\t%s\n",filename,"good");
	return(0);
}
else if(flag==1){
	fprintf(fp_index,"%s\t%s\n",filename,"bad");
	return(0);
}
else	return(101);	

}

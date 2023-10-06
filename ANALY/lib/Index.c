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

�@�\�j
fp_index �� "rb" mode �ŃI�[�v�����ꂽ scan index file �����s��ǂ݁A�t�@�C�����A�t���b�O��������B
�Ԓl�j
	0	:	����I��
	11	:	blank line
	100	:	EOF �����o����
	201	:	field �̐����������Ȃ��i���Ȃ��j
	202	:	field �̐����������Ȃ��i�����j
	301	:	flag �̕\�����������Ȃ�

1992/12/1	���@����
1992/12/11 ���c���O (�t�@�C������c-string���j
*/

{
char	buffer[MAX_IDX_RECORD_LENGTH];
char	filename_buff[MAX_IDX_RECORD_LENGTH];
char	flag_buff[MAX_IDX_RECORD_LENGTH];
char	*token;

/*	�X�g���[������̈�s����	*/

if(fgets(buffer,MAX_IDX_RECORD_LENGTH,fp_index)==NULL)	return(100);

/*	���͂�����������g�[�N���ɋ�؂�	*/
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

/*	flag �����̎���		*/

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

�@�\�j
fp_index �� "wb" mode �ŃI�[�v�����ꂽ scan index file �փt�@�C�����A�t���b�O�l����s�������ށB
�߂�l�j
	0	:	����I��
	101	:	flag �̒l���������Ȃ��B(0 or 1 �ȊO)

1992/12/1	���@����

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

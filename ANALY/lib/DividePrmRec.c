/*
divide prm record into keyword and value by =
omit tab and space

1992/11/27 T.Handa
*/

#include <stdio.h>
#include <profhead.h>
#include <starlib.h>

void STAR_DividePrmRecord(record,keyword,value)
char	record[];
char	keyword[];
char	value[];
{
	static char	delimiter='=';
	int	i_rec,i_key,i_value;

/* find first nonNULL character */
	i_rec=0;
	while(record[i_rec]!=NULL)
	{
		if(record[i_rec]!=' ' && record[i_rec]!='\t') break;
		i_rec++;
	}

/* make keyword */
	i_key=0;
	while(record[i_rec]!=NULL && record[i_rec]!=delimiter)
	{
	/* cutout comment part */
		if(record[i_rec]=='/' && record[i_rec+1]=='*') break;
	/* cutout CR for fgets */
		if(record[i_rec]=='\n') break;

		keyword[i_key]=record[i_rec];
		i_rec++; i_key++;
	}

	keyword[i_key]=NULL;

/* discard additional space & tab */
	i_key--;
	for(;i_key>0;i_key--)
	{
		if(keyword[i_key]!=' ' && keyword[i_key]!='\t') break;
		keyword[i_key]=NULL;
	}

/* make value */
	if(record[i_rec]==delimiter) i_rec++;
	while(record[i_rec]==' ' || record[i_rec]=='\t')
		i_rec++;

	i_value=0;
	while(record[i_rec]!=NULL)
	{
	/* cutout comment part */
		if(record[i_rec]=='/' && record[i_rec+1]=='*') break;
	/* cutout CR for fgets */
		if(record[i_rec]=='\n') break;
		value[i_value]=record[i_rec];
		i_rec++; i_value++;
	}
	value[i_value]=NULL;

/* discard additional space & tab */
	i_value--;
	for(;i_value>0;i_value--)
	{
		if(value[i_value]!=' ' && value[i_value]!='\t') break;
		value[i_value]=NULL;
	}

}

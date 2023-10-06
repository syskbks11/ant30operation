#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <profhead.h>
//#include <strdef.h>
#include <starlib.h>

int STAR_StdFileExtName(env_name,file_name,std_file_name,extension)
/*
add extension file

5001:too long file name

1992/4/29 T.Handa
1992/11/26 T.Handa (revise) add ENV paramter
*/
char *env_name;
char *file_name;
char *std_file_name;
char *extension;
{
	int	l_ext, l_file_name,l_dir;
	char 	*env_p,dir_name[MAX_FILE_NAME_LENGTH];

	strcpy(std_file_name,file_name);
	l_file_name=strlen(std_file_name);
	l_ext=strlen(extension);

/* length check */
	if(l_file_name+l_ext>=MAX_FILE_NAME_LENGTH)
		return(5001);

	if(env_name[0]!=NULL)
	{
/* add ENV parameter */
		env_p=getenv(env_name);
		if(env_p!=NULL && file_name[0]!='/')
		{
			l_dir=strlen(env_p);
			if(l_dir+l_file_name+1>=MAX_FILE_NAME_LENGTH)
				return(5001);
			strcpy(dir_name,env_p);
			if(dir_name[l_dir-1]!='/')
				strcat(dir_name,"/");
			strcpy(std_file_name,dir_name);
			strcat(std_file_name,file_name);

		/* recheck file name length */
			l_file_name=strlen(std_file_name);
			if(l_file_name+l_ext>=MAX_FILE_NAME_LENGTH)
				return(5001);
		}
	}

/* check extension part */
	if(l_file_name>l_ext)
	{
		if(strcmp(&std_file_name[l_file_name-l_ext],extension)!=0)
			strcat(std_file_name,extension);
	}
	else
		strcat(std_file_name,extension);

	return(0);	
}

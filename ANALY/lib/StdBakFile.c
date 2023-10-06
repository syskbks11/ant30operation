#include <stdio.h>
#include <strings.h>

#include <profhead.h>
//#include <strdef.h>
#include <starlib.h>

int STAR_StdBakFileName(org_file_name,bak_file_name,backup_ver,extension)
/*
make backup version name

5001:too long file name

1992/4/30 T.Handa
*/
char	*org_file_name;
char	*bak_file_name;
int	backup_ver;
char	*extension;
{
	int	l_ext, l_file_name;
	char	buffer[5];

	l_file_name=strlen(org_file_name);
	l_ext=strlen(extension);

	if(backup_ver<0 || backup_ver>255)
		return(5102);	/* too large or invalid backup_ver number */

	if(l_file_name<=l_ext)
		return(5101);	/* too short file_name */

	if(strcmp(&org_file_name[l_file_name-l_ext],extension)!=0)
		return(5101);	/* invalid extension */

	strcpy(bak_file_name,org_file_name);
	bak_file_name[l_file_name-l_ext]=NULL;

	sprintf(buffer,".%d",backup_ver);
	if(strlen(buffer)+l_file_name>=MAX_FILE_NAME_LENGTH)
		return(5001);	/* too long file name */

	strcat(bak_file_name,buffer);
	strcat(bak_file_name,extension);

	return(0);
}

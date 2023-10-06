#include <stdlib.h>

#include "glacier_att.h"

short MyGpibAdrs = 1;	// GPIB address for USB-GPIB
char GpAdrsBuf[12];	// GPIB address for target-GPIB

int g_tmout;
int g_eoi;
int g_delm;

int main( int   argc,
          char *argv[] )
{
	int res;
	
	res = glacier_att_init(ATT_MODE_CONNECT);
	if( res != 0){
	}
//	gtk_init (&argc, &argv);

//	create_window();

//	gtk_main ();
	//gp_detach(fd);
	
	res = glacier_att_close();
	if( res != 0){
	}

	return 0;
}

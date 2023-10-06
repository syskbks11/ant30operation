#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../libcssl-0.9.4/cssl.h"
#include "../serial/glacier_serial.h"
#include "../debug/glacier_debug.h"

double convert_returned_voltage(const unsigned char* data)
{
	int rx1, rx2, tmp;
	rx1 = (int)(unsigned char)data[0];
	rx2 = (int)(unsigned char)data[1];
	tmp = rx2 | (rx1 << 8);
	return (double)tmp/1024.0*5.0;
}

double convert_returned_temperature(const unsigned char* data)
{
	int res;
	printf("%d,%d\n", data[0], data[1]);
	res = (int)data[1];
	res = (res << 8) | data[0];
	return (double)res/16.0;
}

int
main(int argc,char *argv[])
{
	//uint8_t buffer[30];
    cssl_t *serial;

	int id=0;

    cssl_start();
	//printf("cssl_t:%d\n", sizeof(cssl_t));
	//printf("glacier_serial_t:%d\n", sizeof(glacier_serial_t));

    
	char *devName;
	if(argc == 2){
		devName = argv[1];
	}else{
		devName = "/dev/ttyS0";
	}
	serial=cssl_open(devName,glacier_serial_callback_FIXED,id,
//    serial=cssl_open(devName,glacier_serial_callback_DEFAULT,id,
		     9600,8,0,1);
//    serial=cssl_open(devName,callback,0,
//		     19200,8,0,1);

    if (!serial) {
	printf("%s\n",cssl_geterrormsg());
	return -1;
    }
	int res = glacier_serial_attach(serial, GLACIER_SERIAL_1ST_NONE);
	//int res = glacier_serial_attach(serial, GLACIER_SERIAL_1ST_ODD);
	if(res){
		printf("already attached. ");
	}

	cssl_putchar(serial,'X');
	printf("> X\n");
	usleep(250*1000);
	//do{
		cssl_putchar(serial,'P');
		printf("> P\n");
		glacier_serial_set_bytes_to_read(1);

		glacier_serial_wait_for_massage_complete(id);
	//}while(glacier_serial_get_massage(id)[0]!='Q');
	printf("### %s ###\n", glacier_serial_get_massage(id));
	usleep(2000*1000);
	//glacier_serial_wait_for_massage_complete(id);
	//printf("### %s ###\n", glacier_serial_get_massage(id));
/*
	cssl_putchar(serial, 'N');
	printf("> N\n");
	glacier_serial_wait_for_massage_complete(id);
	printf("### %s ###\n", glacier_serial_get_massage(id));
*/
	unsigned char* data;
/*
	cssl_putchar(serial, 'A');
	printf("> A\n");
	glacier_serial_set_bytes_to_read(2);
	glacier_serial_wait_for_massage_complete(id);
	data = glacier_serial_get_massage(id);
	printf("### %s ###\n", data);
	printf("### %d ###\n", convert_returned_data(data));
*/
/*
	cssl_putchar(serial, 'B');
	printf("> B\n");
	//glacier_serial_set_bytes_to_read(2);
	glacier_serial_wait_for_massage_complete(id);
	data = glacier_serial_get_massage(id);
	printf("### %s ###\n", data);
	printf("### %d ###\n", convert_returned_data(data));
*/
	cssl_putchar(serial, 'S');
	printf("> S\n");
	glacier_serial_set_bytes_to_read(9);
	glacier_serial_wait_for_massage_complete(id);
	data = glacier_serial_get_massage(id);
	//printf("### %s ###\n", glacier_serial_get_massage(id));
	printf("### %lf deg C ###\n", convert_returned_temperature(data));
	usleep(2000*1000);
/*
	cssl_putchar(serial, 'T');
	printf("> T\n");
	glacier_serial_wait_for_massage_complete(id);
	printf("### %s ###\n", glacier_serial_get_massage(id));

	cssl_putchar(serial, 'U');
	printf("> U\n");
	glacier_serial_wait_for_massage_complete(id);
	printf("### %s ###\n", glacier_serial_get_massage(id));

	cssl_putchar(serial, 'R');
	printf("> R\n");
	glacier_serial_set_bytes_to_read(18);
	glacier_serial_wait_for_massage_complete(id);
	printf("### %s ###\n", glacier_serial_get_massage(id));
	//glacier_serial_wait_for_massage_complete(id);
	//printf("### %s ###\n", glacier_serial_get_massage(id));
*/
//	do{
//		cssl_putstring(serial,"PR1\r\n");
//		printf("> PR1\n");
//		glacier_serial_wait_for_massage_complete(id);
//	}while(glacier_serial_get_massage(id)[0]!=6);
//	cssl_putchar(serial,5);
//	printf("> <ENQ>\n");
//	glacier_serial_wait_for_massage_complete(id);

//	do{
//		cssl_putstring(serial,"COM,1\r\n");
//		printf("> COM,1\n");
//		glacier_serial_wait_for_massage_complete(id);
//	}while(glacier_serial_get_massage(id)[0]!=6);

//	printf("we measure for 10 sec.\n");
//	int i;
//	for (i=0; i < 10; i++){
//		glacier_serial_wait_for_massage_complete(id);
//	}
	//usleep(10000*1000);
//	cssl_putchar(serial,5);
//	printf("> <ENQ>\n");
//	printf("we wait for 3 sec.\n");
//	usleep(3000*1000);


	printf("we exit after 2 sec.\n");
	usleep(2000*1000);
	cssl_close(serial);
	cssl_stop();

	return 0;
}

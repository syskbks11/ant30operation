/**
 * glacier_att.c
 * An implementation of glacier_att.h with USB220. 
 * 
 * written by NAGAI Makoto. 
 * 2008/10/20 start coding. 
 */

#include "glacier_att.h"

#define _USE_U2
#define MAX_USB_DEVICES				256


#define GPIB_ADDRESS_ATTENUATOR 28


int att_initGPIB();
int att_openFirstUSB220();

typedef struct att_s{
	int useMode;	//
	int fd;	// file descriptor of USB220
	short MyGpibAdrs;
	char GpAdrsBuf[12];	// GPIB of the attenuator driver.
}att_t;

static int init=0;
static att_t p;



int g_tmout;
int g_eoi;
int g_delm;

/**
 *
 */
int glacier_att_init(int useMode)
{
	p.useMode = useMode;
	if(p.useMode == ATT_MODE_NO_CONNECT){
		return 0;
	}
	if(p.useMode != ATT_MODE_NO_CONNECT){
		perror("Invalid mode for glacier_att_init()");
		return -1;
	}
	int res;
	
	int fd = att_openFirstUSB220();
	if(fd<=0){
		printf("Device not opened \n");
		return -1;
	}
	p.fd = fd;
	//InitFlag = 0;
	g_tmout = 10;
	g_eoi = 0x0;
	g_delm = 0x0a;
	gp_GetDeviceID(fd, 0, (char*)(&p.MyGpibAdrs));

	res = att_initGPIB();
	if( res !=0 ){
		printf("att_initGPIB() failed.");
		return -1;
	}

	sprintf(p.GpAdrsBuf, "%d", GPIB_ADDRESS_ATTENUATOR);
	//p.GpAdrsBuf = GPIB_ADDRESS_ATTENUATOR;
	return 0;
}


int glacier_att_send(char* buf)
{
	int	GpStatus;
	char TxBuf[1024];
	//int TxLen;
	//gchar *buff;
	int	ret;

	memset(TxBuf, 0x00, sizeof(TxBuf));
	sprintf(TxBuf, "%s", buf);

	printf("CmdTxCommand [%s]\n", TxBuf);
#ifdef _USE_U2
	GpStatus = U2Gpwrt( p.fd, p.GpAdrsBuf, TxBuf );
#else
	GpStatus = gp_wrt(p.GpAdrsBuf, TxBuf);
#endif
	if(GpStatus != 0)
	{
		printf("gp_wrt() error [%d]\n", GpStatus);
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	//TxLen = gp_count();

	//printf("CmdTxCommand() end\n");
	return ret;
}
int glacier_att_receive(char* buf, int bufLen)
{
	int		GpStatus;
	char	RxBuf[1024];
	int		RxLen;

	//printf("CmdRxData() start\n");
	//gtk_statusbar_push (GTK_STATUSBAR (status_bar), GPOINTER_TO_INT (status_id), "受信中・・・");

	RxLen = sizeof(RxBuf);
	memset(RxBuf, 0x00, RxLen);
#ifdef _USE_U2
	GpStatus = U2Gpred( p.fd, p.GpAdrsBuf, RxBuf, RxLen );
#else
	GpStatus = gp_red(p.GpAdrsBuf, RxBuf, RxLen);
#endif
	if(GpStatus != 0)
	{
		printf("gp_red() error [%d]\n", GpStatus);
	}
	else
	{
	}

	RxLen = gp_count();
	//sprintf(szBuf, "%d", RxLen);
	RxBuf[RxLen]='\0';
	printf("CmdRxData[%s]\n", RxBuf);
}
int glacier_att_close()
{
	int	GpStatus;

	if(p.useMode == ATT_MODE_NO_CONNECT){
		//printf("Not initialized yet. \n");
		return 0;
	}

	//send LCL command
#ifdef _USE_U2
	GpStatus = U2Gplcl(p.fd, p.GpAdrsBuf);
#else
	GpStatus = gp_lcl( p.GpAdrsBuf );
#endif
	if ( GpStatus != 0 )
	{
		printf("gp_lcl() error [%d]\n", GpStatus);
	}

	gp_detach(p.fd);
}

/**
 * 
 */
int att_initGPIB()
{
	int		GpStatus;

	// Init GPIB controller
#ifdef _USE_U2
	GpStatus = U2Gpinit( p.fd, p.MyGpibAdrs );
#else
	GpStatus = gp_init( p.MyGpibAdrs, 0, 0 );
#endif
	if( GpStatus != 0 )
	{
		printf("gp_init() error [%d]\n", GpStatus);
		return -1;
	}

	// enable IFC
#ifdef _USE_U2
	GpStatus = U2Gpcli(p.fd);
#else
	GpStatus = gp_cli();
#endif
	if ( GpStatus != 0 )
	{
		printf("gp_cli() error [%d]\n", GpStatus);
		return -1;
	}

	// enable REN
#ifdef _USE_U2
	GpStatus = U2Gpren(p.fd);
#else
	GpStatus = gp_ren();
#endif
	if ( GpStatus != 0 )
	{
		printf("gp_ren() error [%d]\n", GpStatus);
		return -1;
	}

	// Send SDC command
	// get address of GPIBdevice
	if(p.GpAdrsBuf[0] != 0){
#ifdef _USE_U2
		GpStatus = U2Gpclr(p.fd, p.GpAdrsBuf);
#else
		GpStatus = gp_clr( p.GpAdrsBuf );
#endif
	}else{
#ifdef _USE_U2
		GpStatus = U2Gpclr(p.fd, "");
#else
		GpStatus = gp_clr( "" );
#endif
	}
	if ( GpStatus != 0 )
	{
		printf("gp_clr() error [%d]\n", GpStatus);
		return -1;
	}

	//gtk_statusbar_push (GTK_STATUSBAR (status_bar), GPOINTER_TO_INT (status_id), "初期化正常終了");
	//InitFlag = 1;
	
	return 0;//normal end
}

/**
 * 
 * @return file discriptor of USB220. 
 * 0 or negative values will be returned if failed. 
 */
int att_openFirstUSB220()
{
	int fd;
#ifdef _USE_U2
	int i;
	unsigned short UnitId[MAX_USB_DEVICES];
	unsigned short MaxUnit = MAX_USB_DEVICES;

	// 引数無しの場合
	MaxUnit = U2GpEnumUnit(UnitId, MaxUnit);	for(i=0;i<MaxUnit;i++){
		printf("U2GpEnumUnit%d UnitId = %02x \n", i, UnitId[i]);
	}
	//最初に見つけたUSB220をオープン
	fd=U2GpOpenUnit(UnitId[0]);
	printf("U2GpEnumUnit%d UnitId = %02x try to open\n", 0, UnitId[0]);//081020
#else
	fd=gp_attach();
#endif
	return fd;
}


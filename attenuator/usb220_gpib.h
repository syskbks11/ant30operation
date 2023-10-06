#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<errno.h>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<sys/types.h>
#include	<pthread.h>



// IOCTL for Linux
#define GPIB_SEND_COMMAND		0x1
#define GPIB_RECV_COMMAND		0x2
#define GPIB_START_INTERRUPT	0x3
#define GPIB_STOP_INTERRUPT		0x4
//#define GPIB_RESET				0x5
#define GPIB_ABORT_PIPE			0x6
//#define GPIB_CLEAR_STALL		0x7
//#define GPIB_DELAY				0x8



struct GpibCommand {
	short length;
	int request;
	int requesttype;
	int value;
	int index;
	void *buffer;
	int timeout;
};

// Common vendor request for all unit
#define		Vendor_bRequest_Get_DeviceId	0x01
#define		Vendor_bRequest_Get_FWVersion	0x04
#define		Vendor_bRequest_Get_HWVersion	0x05

// Vendor request for GP-IB unit
#define		Vendor_bRequest_GP_GETSTATUS	0x60
#define		Vendor_bRequest_GP_INIT			0x61
#define		Vendor_bRequest_GP_CLI			0x62
#define		Vendor_bRequest_GP_REN			0x63
#define		Vendor_bRequest_GP_CLR			0x64
#define		Vendor_bRequest_GP_WRT			0x65
#define		Vendor_bRequest_GP_RED			0x66
#define		Vendor_bRequest_GP_TFRIN			0x67
#define		Vendor_bRequest_GP_TRG			0x68
#define		Vendor_bRequest_GP_TFRINIT		0x69
#define		Vendor_bRequest_GP_TFRINS		0x6A
#define		Vendor_bRequest_GP_TFREND		0x6B
#define		Vendor_bRequest_GP_TFROUT		0x6C
#define		Vendor_bRequest_GP_LCL			0x6D
#define		Vendor_bRequest_GP_LLO			0x6E
#define		Vendor_bRequest_GP_WTB			0x6F
#define		Vendor_bRequest_GP_RDS			0x70
#define		Vendor_bRequest_GP_RDS1			0x71
#define		Vendor_bRequest_GP_STARTSRQ		0x72
#define		Vendor_bRequest_GP_STOPSRQ		0x73
//#define		Vendor_bRequest_GP_STARTWSRQB	0x74
#define		Vendor_bRequest_GP_DELM			0x75
#define		Vendor_bRequest_GP_TIMEOUT		0x76
#define		Vendor_bRequest_GP_SETDELAY		0x77

#define		Vendor_bRequest_GP_MYADRS		0x78
#define		Vendor_bRequest_GP_RED1			0x79
#define		Vendor_bRequest_GP_RED2			0x7A
#define		Vendor_bRequest_GP_REDRST		0x7B
#define		Vendor_bRequest_GP_TFROUTUNT	0x00
//#define		Vendor_bRequest_GP_STARTWSRQ	0x74

#define		Vendor_bRequest_GP_OUTREG		0x7E
#define		Vendor_bRequest_GP_INREG			0x7F

#define		Vendor_bRequest_GP_WRT1			0x80
#define		Vendor_bRequest_GP_TFROUT1		0x81

#define		Vendor_bRequest_GP_COUNT			0xCA




typedef void (*PAPIFUNC)(void);

extern unsigned char ClearFeatureAfterStallCheck(unsigned char ep);

int gp_attach();
int gp_detach(int num);

int gp_GetDeviceID(int hDev, unsigned short UnitId, char* pDevId);
unsigned short OutPort(short Reg, short bOutVal);
unsigned short InPort(short Reg);
int gp_wtb(char * buf);
int gp_dataout(char* buf, int bufLen, unsigned char DMAflag);
int gp_datain(char* buf, int bufLen, unsigned char DMAflag);
int gp_datain_End(void);
int gp_init(short GpAdrs, short IOBase, short IrqNo);
int gp_cli(void);
int gp_ren(void);
int gp_tmout(int SecTime);
int gp_delay(int UsecTime);
int gp_setdelay(int DelayTime);
int gp_clr(char * adrs);
int gp_myadr(void);
int gp_srqCheck(void);

int gp_wrt(char* adrs, char* buf);
int gp_wrtd(char* buf, int bufLenOrg);
void gp_wait(int WaitSecTime);
int gp_trg(char* adrs);
int gp_wsrq(int WaitSecTime);
int gp_wsrqb(int WaitSecTime);

int gp_rds(char* adrs, unsigned char* status_byte);
int gp_rds1(char* adrs, unsigned char* status_byte);
int gp_red(char* adrs, char* buf, int bufLen);
int gp_redd(char* buf, int bufLen);
int gp_redah(char* adrs, char* buf, int bufLen);
int gp_redrst(void);

int gp_lcl(char* adrs);
int gp_llo(void);
int gp_count(void);
int gp_delm(char *mode, unsigned int dlm);
void gp_strtodbl(unsigned char *bPoint, double *val);
void gp_strtoflt(unsigned char *bPoint, float *val);
int gp_tfrout(char *adrs, int bufLen, char *buf);
int gp_tfroutd(int bufLen, char *buf);
int gp_tfrout_unt(char *adrs, int bufLen, char *buf);
int gp_tfrin(char *adrs, int bufLen, char *buf);
int gp_tfrinit(char *adrs);
int gp_tfrins(int bufLen, char *buf);
void gp_tfrend(void);
int gp_findlstn(char *adrs, int adrsLen);

int U2GpOpenUnit(unsigned short UnitId);
int U2GpCloseUnit(int hDriver);
int U2GpEnumUnit(unsigned short *pUnitId, unsigned short MaxUnit);
unsigned short U2OutPort(int hDriver, unsigned short Reg, unsigned short bOutVal);
unsigned short U2InPort(int hDriver, unsigned short Reg);
int U2Gpinit(int hDriver, unsigned short GpAdrs);
int U2Gpcli(int hDriver);
int U2Gpren(int hDriver);
int U2Gpclr(int hDriver, char *adrs);
int U2Gpmyadr(int hDriver);
int U2Gpwrt(int hDriver, char *adrs, char *buf);
int U2Gptrg(int hDriver, char *adrs);
int U2Gpwsrq(int hDriver, int WaitSecTime);
int U2Gpwsrqb(int hDriver, int WaitSecTime);
int U2Gprds(int hDriver, char *adrs, unsigned char *status_byte);
int U2Gprds1(int hDriver, char *adrs, unsigned char *status_byte);
int U2Gpred(int hDriver, char *adrs, char *buf, int bufLen);
int U2Gpredd(int hDriver, char *buf, int bufLen);
int U2Gpredah(int hDriver, char *adrs, char *buf, int bufLen);
int U2Gpredrst(int hDriver);
int U2Gpsrq(int hDriver, int SrqMode, PAPIFUNC pFunc);
int U2Gplcl(int hDriver, char *adrs);
int U2Gpllo(int hDriver);
int U2Gptmout(int hDriver, int SecTime);
int U2Gpsetdelay(int hDriver, int DelayTime);
int U2Gpcount(int hDriver);
int U2Gpdelm(int hDriver, char *mode, unsigned int dlm);
int U2Gptfrout(int hDriver, char *adrs, int bufLen, char *buf);
int U2Gptfrout_unt(int hDriver, char *adrs, int bufLen, char *buf);
int U2Gptfrin(int hDriver, char *adrs, int bufLen, char *buf);
int U2Gptfrinit(int hDriver, char *adrs);
int U2Gptfrins(int hDriver, int bufLen, char *buf);
void U2Gptfrend(int hDriver);
int U2Gpwtb(int hDriver, char *buf);

int U2GpsrqCheck(int hDriver);
int U2Gpwrtd(int hDriver, char *buf, int bufLen);
int U2Gptfroutd(int hDriver, int bufLen, char *buf);
int U2Gpfindlstn(int hDriver, char *adrs, int adrsLen);


int gp_srq(int SrqMode, PAPIFUNC pFunc);



#if 0
#define DllExport extern "C" __declspec (dllexport)
DllExport WORD APIENTRY OutPort( WORD Reg, WORD bOutVal );
DllExport WORD APIENTRY InPort( WORD Reg );
DllExport int APIENTRY gp_init( WORD GpAdrs, WORD IOBase, WORD IrqNo );
DllExport int APIENTRY gp_cli( void );
DllExport int APIENTRY gp_ren( void );
DllExport int APIENTRY gp_clr( PCHAR adrs );
DllExport int APIENTRY gp_myadr( void );
DllExport int APIENTRY gp_wrt( PCHAR adrs, PCHAR buf );
DllExport void APIENTRY gp_wait( int WaitSecTime );
DllExport int APIENTRY gp_trg( PCHAR adrs );
DllExport int APIENTRY gp_wsrq( int WaitSecTime );
DllExport int APIENTRY gp_wsrqb( int WaitSecTime );
DllExport int APIENTRY gp_rds( PCHAR adrs, PUCHAR status_byte );
DllExport int APIENTRY gp_rds1( PCHAR adrs, PUCHAR status_byte );
DllExport int APIENTRY gp_red( PCHAR adrs, PCHAR buf, int bufLen );
DllExport int APIENTRY gp_srq( HWND hWnd, INT SrqMode );
DllExport int APIENTRY gp_lcl( PCHAR adrs );
DllExport int APIENTRY gp_llo( void );
DllExport int APIENTRY gp_tmout( int SecTime );
DllExport int APIENTRY gp_delay( DWORD UsecTime );
DllExport int APIENTRY gp_setdelay( int DelayTime );
DllExport int APIENTRY gp_count( void );
DllExport int APIENTRY gp_delm( char *mode, unsigned int dlm );
DllExport void APIENTRY gp_strtodbl( BYTE *bPoint, double *val );
DllExport void APIENTRY gp_strtoflt( BYTE *bPoint, float *val );
DllExport int APIENTRY gp_tfrout( PCHAR adrs, int bufLen, PCHAR buf );
DllExport int APIENTRY gp_tfrout_unt( PCHAR adrs, int bufLen, PCHAR buf );
DllExport int APIENTRY gp_tfrin( PCHAR adrs, int bufLen, PCHAR buf );
DllExport int APIENTRY gp_tfrinit( PCHAR adrs );
DllExport int APIENTRY gp_tfrins( int bufLen, PCHAR buf );
DllExport void APIENTRY gp_tfrend( void );
DllExport int APIENTRY gp_wtb( PCHAR buf );

BYTE wsrq( void );
BYTE wsrqb( void );
unsigned __stdcall StartInterruptThread(LPVOID pContext);

////////////////////////////////////////////////////////////////////////
// U2xxx
DllExport HANDLE APIENTRY U2GpOpenUnit( USHORT UnitId );
DllExport VOID APIENTRY U2GpCloseUnit( HANDLE hUnit );
DllExport INT APIENTRY U2GpEnumUnit( PUSHORT pUnitId, USHORT MaxUnit );
DllExport WORD APIENTRY U2OutPort( HANDLE hUnit, WORD Reg, WORD bOutVal );
DllExport WORD APIENTRY U2InPort( HANDLE hUnit, WORD Reg );
DllExport int APIENTRY U2Gpinit( HANDLE hUnit, WORD GpAdrs );
DllExport int APIENTRY U2Gpcli( HANDLE hUnit );
DllExport int APIENTRY U2Gpren( HANDLE hUnit );
DllExport int APIENTRY U2Gpclr( HANDLE hUnit, PCHAR adrs );
DllExport int APIENTRY U2Gpmyadr( HANDLE hUnit );
DllExport int APIENTRY U2Gpwrt( HANDLE hUnit, PCHAR adrs, PCHAR buf );
DllExport int APIENTRY U2Gptrg( HANDLE hUnit, PCHAR adrs );
DllExport int APIENTRY U2Gpwsrq( HANDLE hUnit, int WaitSecTime );
DllExport int APIENTRY U2Gpwsrqb( HANDLE hUnit, int WaitSecTime );
DllExport int APIENTRY U2Gprds( HANDLE hUnit, PCHAR adrs, PUCHAR status_byte );
DllExport int APIENTRY U2Gprds1( HANDLE hUnit, PCHAR adrs, PUCHAR status_byte );
DllExport int APIENTRY U2Gpred( HANDLE hUnit, PCHAR adrs, PCHAR buf, int bufLen );
DllExport int APIENTRY U2Gpsrq( HANDLE hUnit, HWND hWnd, INT SrqMode );
DllExport int APIENTRY U2Gplcl( HANDLE hUnit, PCHAR adrs );
DllExport int APIENTRY U2Gpllo( HANDLE hUnit );
DllExport int APIENTRY U2Gptmout( HANDLE hUnit, int SecTime );
DllExport int APIENTRY U2Gpsetdelay( HANDLE hUnit, int DelayTime );
DllExport int APIENTRY U2Gpcount( HANDLE hUnit );
DllExport int APIENTRY U2Gpdelm( HANDLE hUnit, char *mode, unsigned int dlm );
DllExport int APIENTRY U2Gptfrout( HANDLE hUnit, PCHAR adrs, int bufLen, PCHAR buf );
DllExport int APIENTRY U2Gptfrout_unt( HANDLE hUnit, PCHAR adrs, int bufLen, PCHAR buf );
DllExport int APIENTRY U2Gptfrin( HANDLE hUnit, PCHAR adrs, int bufLen, PCHAR buf );
DllExport int APIENTRY U2Gptfrinit( HANDLE hUnit, PCHAR adrs );
DllExport int APIENTRY U2Gptfrins( HANDLE hUnit, int bufLen, PCHAR buf );
DllExport void APIENTRY U2Gptfrend( HANDLE hUnit );
DllExport int APIENTRY U2Gpwtb( HANDLE hUnit, PCHAR buf );

INT gp_GetDeviceID( HANDLE hDev, USHORT UnitId, PUCHAR pDevId);

//add
DllExport int APIENTRY gp_srqCheck(void);
DllExport int APIENTRY gp_wrtd(PCHAR buf, int bufLen);
DllExport int APIENTRY gp_tfroutd(int bufLen, PCHAR buf);
DllExport int APIENTRY gp_redd(PCHAR buf, int bufLen);
DllExport int APIENTRY gp_redah(PCHAR adrs, PCHAR buf, int bufLen);
DllExport int APIENTRY gp_redrst(void);
DllExport int APIENTRY U2GpsrqCheck(HANDLE hUnit);
DllExport int APIENTRY U2Gpwrtd(HANDLE hUnit, PCHAR buf, int bufLen);
DllExport int APIENTRY U2Gptfroutd(HANDLE hUnit, int bufLen, PCHAR buf);
DllExport int APIENTRY U2Gpredd(HANDLE hUnit, PCHAR buf, int bufLen);
DllExport int APIENTRY U2Gpredah(HANDLE hUnit, PCHAR adrs, PCHAR buf, int bufLen);
DllExport int APIENTRY U2Gpredrst(HANDLE hUnit);

DllExport int APIENTRY gp_findlstn(PCHAR adrs, int adrsLen);
DllExport int APIENTRY U2Gpfindlstn(HANDLE hUnit, PCHAR adrs, int adrsLen);

//local
char cah(BYTE *Data);
char csh(BYTE Data);
void SetTimeOut1(DWORD sec);
BYTE IsTimeOut1(void);
BYTE ovDevIOCtl(SETUP_PACKET *SetupPkt);
void GetUnitIdFromhDrv(HANDLE hUnit);
int gp_datain(PCHAR buf, DWORD bufLen, BYTE DMAflag);
int gp_dataout(PCHAR buf, DWORD bufLen, BYTE DMAflag);
int gp_datain_End(void);
DWORD GetHSCount(void);

#endif


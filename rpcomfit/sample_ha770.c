#ifndef INCLUDE_GUARD_UUID_FEF9E298_5978_4039_9BAC_9646FD2D406F
#define INCLUDE_GUARD_UUID_FEF9E298_5978_4039_9BAC_9646FD2D406F
/*--------------------------------------------------------------------------*/
/*  FactoryIT RP-COM(FIT), FX-DS110-COM,RP-COM(FIT)H                        */
/*  仮想COMﾓｰﾄﾞ ﾌﾟﾛｸﾞﾗﾐﾝｸﾞ ｻﾝﾌﾟﾙ ｿｰｽｺｰﾄﾞ    Ver1.02 (2022-07-14)            */
/*                                                                          */
/*  機能:               FactoryIT機器への送信                               */
/*                                                                          */
/*  作成方法:           Windows の場合                                      */
/*                      Microsoft Visual Studio 6.0 を使用                  */
/*                          cl -c -DWIN32 tx.c                              */
/*                          link tx.obj wsock32.lib                         */
/*                      Linux の場合                                        */
/*                          cc tx.c -o tx                                   */
/*                                                                          */
/*                                                      CONTEC Co.,Ltd.     */
/*--------------------------------------------------------------------------*/

#include "fitprot.h"
#include "rpcomfit.h"

#define IP_ADDRESS  "192.168.10.3"
#define HEADER 0x02
#define TRAILER 0x04
//#define VERBOSE 1

char dataWord[FIT_WORDSIZE];

void MotorDataSendReceive(rpcomfitctrl* ctrl, char* rawdataWord, char* recvWord, const unsigned int verbose){
  memset(dataWord, 0x00, strlen(dataWord));
  sprintf(dataWord,"%c%s%c", HEADER,rawdataWord,TRAILER);
  DataSendReceiveVerbose(ctrl, dataWord, recvWord, verbose);
  return;
}

int main(int argc, char *argv[]) {
  if(argc<3 || argc>4){
    printf("Invalid command format. \n --> %s [DEVICE-ID: 1-7] [COMMAND] [VERBOSE=0or1 if needed]\n",argv[0]);
    return -1;
  }
  unsigned int verbose = 0;
  if(argc==4) verbose = atoi(argv[3]);
  //printf("VERBOSE = %d",verbose);

  int devid =  atoi(argv[1]);
  char recvWord[FIT_WORDSIZE];
  SER_DEVCTL iomap;

  if(verbose) printv("[SocketOpen]");
  rpcomfitctrl ctrl;
  rpcomfitctrl* pctrl = &ctrl;
  pctrl->deviceid = devid;
  SocketOpen(pctrl, IP_ADDRESS);

  if( pctrl->socket != -1 ) {

    if(verbose) printv("[FitClose]");
    FitClose(pctrl);
    if(verbose) printv("[FitOpen]");
    FitOpen(pctrl, 9600, 0, 2, 0);
    if(verbose) printv("[FitStatus]");
    FitStatus(pctrl, &iomap);
    //if(verbose)printv_iomap(&iomap);

    if(verbose) printv("[DataSendReceive]");
    //MotorDataSendReceive(pctrl,"VF=10",recvWord);
    MotorDataSendReceive(pctrl,argv[2],recvWord, verbose);

    if(verbose) printv("[FitClose]");
    FitClose(pctrl);

    if(verbose) printv("[SocketClose]");
    SocketClose(pctrl);

  }

  return 0;
}

#endif


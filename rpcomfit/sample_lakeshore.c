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

char dataWord[WORDSIZE];

void LakeshoreDataSendReceive(p_rpcomfitctrl ctrl, char* rawdataWord, char* recvWord){
  memset(dataWord, 0x00, strlen(dataWord));
  sprintf(dataWord,"%s\r\n", rawdataWord);
  DataSendReceive(ctrl, dataWord, recvWord);
  return;
}

int main(int argc, char *argv[]) {
  if(argc!=3){
    printf("Invalid command format. \n --> %s [DEVICE-ID: 1-7] [COMMAND]\n",argv[0]);
    return -1;
  }
  int devid =  atoi(argv[1]);
  char recvWord[WORDSIZE];
  SER_DEVCTL iomap;

  printv("[SocketOpen]");
  rpcomfitctrl ctrl;
  p_rpcomfitctrl pctrl = &ctrl;
  pctrl->deviceid = devid;
  SocketOpen(pctrl, IP_ADDRESS);

  if( pctrl->socket != -1 ) {

    printv("[FitClose]");
    FitClose(pctrl);
    printv("[FitOpen]");
    FitOpen(pctrl, 9600, 0, 2, 0);
    printv("[FitStatus]");
    FitStatus(pctrl, &iomap);

    printv("[DataSendReceive]");
    //LakeshoreDataSendReceive(pctrl,"VF=10",recvWord);
    LakeshoreDataSendReceive(pctrl,argv[2],recvWord);

    printv("[FitClose]");
    FitClose(pctrl);

    printv("[SocketClose]");
    SocketClose(pctrl);

  }

  return 0;
}

#endif

